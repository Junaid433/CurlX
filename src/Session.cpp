#include "CurlX/Session.hpp"
#include "CurlX/Response.hpp"
#include "CurlX/Request.hpp"
#include "CurlX/Headers.hpp"
#include "CurlX/Params.hpp"
#include "CurlX/Files.hpp"
#include "CurlX/Cookies.hpp"
#include "CurlX/Auth.hpp" // Include Auth.hpp
#include "CurlX/Exceptions.hpp" // Include the new exceptions header
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <cstdio> // For FILE and fopen/fclose

namespace CurlX {

    // Helper function for URL encoding (copied from Client.cpp)
    std::string url_encode(const std::string& value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : value) {
            if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << '%' << std::setw(2) << (int)((unsigned char)c);
            }
        }
        return escaped.str();
    }

    // Callbacks (copied from Client.cpp)
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* s) {
        size_t newLength = size * nmemb;
        try {
            s->append((char*)contents, newLength);
        }
        catch (std::bad_alloc& e) {
            //handle memory problem
            return 0;
        }
        return newLength;
    }

    static size_t file_write_callback(void* contents, size_t size, size_t nmemb, FILE* stream) {
        return fwrite(contents, size, nmemb, stream);
    }

    static size_t header_callback(char* buffer, size_t size, size_t nitems, HEADERS* headers) {
        std::string header(buffer, size * nitems);
        if (header.length() > 2) { // Ignore empty lines
            headers->add(header.substr(0, header.length() - 2)); // remove \r\n
        }
        return size * nitems;
    }

    Session::Session() : curl_handle(curl_easy_init()) {
    if (!curl_handle) {
        std::cerr << "Error: Failed to initialize CURL in Session constructor." << std::endl;
    }
    // Enable cookie engine by default (in-memory)
    curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, "");
    // Enable automatic content decoding (gzip, deflate, etc.)
    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
}

void Session::set_cookie_jar(const std::string& file_path) {
    cookie_jar_path = file_path;
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, cookie_jar_path.c_str());
        curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, cookie_jar_path.c_str());
    }
}

Session::~Session() {
    if (curl_handle) {
        // If cookie_jar_path is set, ensure cookies are saved
        if (!cookie_jar_path.empty()) {
            curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, cookie_jar_path.c_str());
        }
        curl_easy_cleanup(curl_handle);
    }
}

void Session::set_default_headers(const HEADERS& headers) {
    default_headers = headers;
}

void Session::set_default_cookies(const COOKIES& cookies) {
    default_cookies = cookies;
}

CURL* Session::get_curl_handle() {
    return curl_handle;
}

CurlX::RESPONSE Session::send(const REQUEST& request) {
        CURLcode res;
        RESPONSE response;
        std::string response_body;
        HEADERS response_headers;
        curl_mime *mime = nullptr;
        FILE* output_file = nullptr;

        if (curl_handle) {
            // Reset options for each request to avoid state leakage
            curl_easy_reset(curl_handle);

            std::string full_url = request.get_url().toString();

            // Append query parameters if any
            if (!request.get_params().get().empty()) { // Use .get()
                full_url += "?";
                bool first_param = true;
                for (const auto& pair : request.get_params().get()) { // Use .get()
                    if (!first_param) {
                        full_url += "&";
                    }
                    full_url += url_encode(pair.first) + "=" + url_encode(pair.second);
                    first_param = false;
                }
            }

            curl_easy_setopt(curl_handle, CURLOPT_URL, full_url.c_str());
            curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, request.get_method().c_str());

            // Handle file uploads using curl_mime API
            if (!request.files_.get().empty()) { // Use .get()
                mime = curl_mime_init(curl_handle);
                for (const auto& file_pair : request.files_.get()) { // Use .get()
                    curl_mimepart *part = curl_mime_addpart(mime);
                    curl_mime_name(part, file_pair.first.c_str());
                    curl_mime_filedata(part, file_pair.second.c_str());
                }
                // Add other form fields (from request.params_) as regular mime parts if needed
                for (const auto& field_pair : request.params_.get()) { // Use .get()
                    curl_mimepart *part = curl_mime_addpart(mime);
                    curl_mime_name(part, field_pair.first.c_str());
                    curl_mime_data(part, field_pair.second.c_str(), CURL_ZERO_TERMINATED);
                }
                curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, mime);
            } else if (!request.body_.toString().empty()) {
                // Only set POSTFIELDS if no files are being uploaded
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request.body_.c_str());
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, request.body_.length());
            }

            // Handle streaming to file or writing to string
            if (!request.output_file_path_.empty()) {
                output_file = fopen(request.output_file_path_.c_str(), "wb");
                if (!output_file) {
                    throw RequestException("Failed to open output file for writing: " + request.output_file_path_);
                }
                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, file_write_callback);
                curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, output_file);
            } else if (request.method_.value == "HEAD") {
                curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L); // Do not download the body
                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL); // No write callback for body
                curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL); // No write data for body
            } else {
                curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 0L); // Ensure body is downloaded for other methods
                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
                curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response_body);
            }

            curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_callback);
            curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &response_headers);

            // Merge request headers with default session headers
            HEADERS effective_headers = default_headers;
            for (const auto& header_line : request.headers_.all()) {
                effective_headers.add(header_line);
            }
            struct curl_slist* chunk = effective_headers.to_curl_slist();
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, chunk);

            // Merge request cookies with default session cookies
            COOKIES effective_cookies = default_cookies;
            for (const auto& cookie_pair : request.cookies_.all()) {
                effective_cookies.add(cookie_pair.first, cookie_pair.second);
            }
            // Set cookies for the request
            for (const auto& cookie_pair : effective_cookies.all()) {
                std::string cookie_string = cookie_pair.first + "=" + cookie_pair.second;
                curl_easy_setopt(curl_handle, CURLOPT_COOKIELIST, cookie_string.c_str());
            }

            // Handle Authentication
            if (request.auth_.type() != AuthType::None) {
                curl_easy_setopt(curl_handle, CURLOPT_USERPWD, request.auth_.user_pass_string().c_str());
                if (request.auth_.type() == AuthType::Basic) {
                    curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
                } else if (request.auth_.type() == AuthType::Digest) {
                    curl_easy_setopt(curl_handle, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
                }
            }

            // Handle Redirects
            if (request.allow_redirects_.allow()) {
                curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, request.allow_redirects_.getMaxRedirects());
            } else {
                curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);
            }

            // TODO: Add timeout, proxy, verify options
            // These will need to be set on curl_handle using request.timeout, request.auth, etc.

            res = curl_easy_perform(curl_handle);

            if (res == CURLE_OK) {
                long response_code;
                curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
                response.statusCode = response_code;
                response.body = response_body; // Will be empty if streamed to file
                response.headers = response_headers;
                response.request_url = request.url_; // Populate request_url
                response.request_headers = effective_headers; // Populate request_headers

                // Populate elapsed_time
                double total_time;
                curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME, &total_time);
                response.elapsed_time = total_time;

                // Populate received_cookies
                // Iterate through response_headers to find "Set-Cookie" headers
                for (const auto& header_line : response_headers.all()) {
                    if (header_line.rfind("Set-Cookie:", 0) == 0) { // Check if it starts with "Set-Cookie:"
                        // Parse the cookie string (e.g., "Set-Cookie: name=value; Path=/")
                        // This is a simplified parsing, a full cookie parser would be more complex
                        std::string cookie_str = header_line.substr(strlen("Set-Cookie: "));
                        size_t eq_pos = cookie_str.find('=');
                        if (eq_pos != std::string::npos) {
                            std::string cookie_name = cookie_str.substr(0, eq_pos);
                            size_t semicolon_pos = cookie_str.find(';', eq_pos);
                            std::string cookie_value = cookie_str.substr(eq_pos + 1, semicolon_pos - (eq_pos + 1));
                            response.received_cookies.add(cookie_name, cookie_value);
                        }
                    }
                }

                // Populate history (simplified: only final effective URL if redirects occurred)
                long redirect_count = 0;
                curl_easy_getinfo(curl_handle, CURLINFO_REDIRECT_COUNT, &redirect_count);
                if (redirect_count > 0) {
                    char* effective_url_cstr = nullptr;
                    curl_easy_getinfo(curl_handle, CURLINFO_EFFECTIVE_URL, &effective_url_cstr);
                    if (effective_url_cstr) {
                        response.history.push_back(URL(effective_url_cstr));
                    }
                }
            }
            else {
                // Handle cURL errors by throwing exceptions
                std::string error_message = curl_easy_strerror(res);
                switch (res) {
                    case CURLE_COULDNT_CONNECT:
                    case CURLE_COULDNT_RESOLVE_HOST:
                        throw ConnectionError(error_message);
                    case CURLE_OPERATION_TIMEDOUT:
                        throw Timeout(error_message);
                    case CURLE_TOO_MANY_REDIRECTS:
                        throw TooManyRedirects(error_message);
                    default:
                        throw RequestException(error_message);
                }
            }

            curl_slist_free_all(chunk);
            if (mime) {
                curl_mime_free(mime);
            }
            if (output_file) {
                fclose(output_file);
            }
        }

        return response;
    }

CurlX::RESPONSE Session::GET(const URL& url, const PARAMS& params, const HEADERS& headers, const COOKIES& cookies, const TIMEOUT& timeout, const AUTH& auth, const PROXY& proxy, const REDIRECTS& redirects, const VERIFY& verify) {
    REQUEST request(url, METHOD::GET, headers, BODY(), timeout, auth, proxy, cookies, redirects, verify, params);
    return send(request);
}

} // namespace CurlX

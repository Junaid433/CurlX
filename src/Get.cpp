#include "CurlX/Get.hpp" // This will include the template function
#include "CurlX/Client.hpp"
#include "CurlX/Request.hpp"
#include "CurlX/Params.hpp" // Added for CurlX::PARAMS
#include "CurlX/Files.hpp"  // Added for CurlX::FILES

namespace CurlX {

    // Implementations of apply_option for each type
    template<>
    void apply_option<HEADERS>(REQUEST& request, const HEADERS& headers) {
        request.headers(headers);
    }

    template<>
    void apply_option<BODY>(REQUEST& request, const BODY& body) {
        request.body(body);
    }

    template<>
    void apply_option<TIMEOUT>(REQUEST& request, const TIMEOUT& timeout) {
        request.timeout(timeout);
    }

    template<>
    void apply_option<AUTH>(REQUEST& request, const AUTH& auth) {
        request.auth(auth);
    }

    template<>
    void apply_option<PROXY>(REQUEST& request, const PROXY& proxy) {
        request.proxy(proxy);
    }

    template<>
    void apply_option<COOKIES>(REQUEST& request, const COOKIES& cookies) {
        request.cookies(cookies);
    }

    template<>
    void apply_option<REDIRECTS>(REQUEST& request, const REDIRECTS& allow_redirects) {
        request.redirects(allow_redirects);
    }

    template<>
    void apply_option<VERIFY>(REQUEST& request, const VERIFY& verify) {
        request.verify(verify);
    }

    template<>
    void apply_option<PARAMS>(REQUEST& request, const PARAMS& params) { // Changed to PARAMS
        request.params(params);
    }

    template<>
    void apply_option<FILES>(REQUEST& request, const FILES& files) { // Changed to FILES
        request.files(files);
    }

} // namespace CurlX

#include <CurlX/Session.hpp>
#include <CurlX/Request.hpp>
#include <iostream>

int main() {
    CurlX::Session session;
    CurlX::REQUEST req;
    req.url("https://httpbin.org/get").method(CurlX::METHOD::GET);
    CurlX::RESPONSE resp = session.send(req);
    if (resp.ok()) {
        std::cout << resp.text() << std::endl;
    } else {
        std::cerr << "Error: " << resp.statusCode << std::endl;
    }
    return 0;
}
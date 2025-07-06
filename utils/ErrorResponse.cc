// In utils/ErrorResponse.cc
#include <drogon/HttpResponse.h>

std::shared_ptr<drogon::HttpResponse> makeErrResp(const std::string& msg) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k500InternalServerError);
    resp->setBody(msg);
    return resp;
}
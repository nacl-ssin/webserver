//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_PROTOCOL_H
#define WEBSERVER_PROTOCOL_H

#include <string>
#include <vector>

class HttpRequest {
private:
    std::string method_;         /*请求方法*/
    std::string path_;           /*请求路径*/
    std::string request_header_; /*请求头*/
    std::string request_body_;   /*请求正文*/
};


class HttpResponse {
private:
    std::string version_;                      /*响应HTTP版本*/
    std::string code_;                         /*响应状态码*/
    std::string code_msg_;                     /*响应状态码描述*/
    std::vector<std::string> response_header_; /*响应头*/
    std::string body_;                         /*响应正文*/
};

#endif //WEBSERVER_PROTOCOL_H

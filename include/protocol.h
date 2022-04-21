//
// Created by 26372 on 2022/4/17.
//

#ifndef WEBSERVER_PROTOCOL_H
#define WEBSERVER_PROTOCOL_H

#include <string>
#include <vector>
#include <unordered_map>

struct HttpData {
    std::string method_;    /*方法*/
    std::string version_;   /*版本*/
    std::unordered_map<std::string, std::string> header_;   /*请求头*/
    std::string body_;      /*正文*/
};

class HttpRequest : public HttpData {
public:
    std::string url_;                          /*请求路径*/
};


class HttpResponse : public HttpData {
public:
    std::string code_;                         /*响应状态码*/
    std::string code_msg_;                     /*响应状态码描述*/
};

#endif //WEBSERVER_PROTOCOL_H

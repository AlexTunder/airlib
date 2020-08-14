//default paths
#define HTTP_404_FN "404.html"
#define HTTP_404_EX(file) SocketException("localhost:server","Error file not found", (char*)file, int(HTTP_LIB), int((0x01a<<4)&404))
#define HTTP_IUK_EX SocketException("localhost:configure request", "Unknowen method", NULL, int(HTTP_LIB), int((0x01a<<4)&500))
#define HTTP_INF_EX SocketException("localhost:set value", "NULL fieldname of value", NULL, int(HTTP_LIB), int((0x01a<<4)&500))
#define HTTP_500_FN "$none"

#pragma once
#include <../Socket.hpp>

#ifndef HTTP_LIB 0x01a
#ifndef HTTP_BITRATE
    #define HTTP_BITRATE 3072
#endif
#define HTTP 1.1

enum HttpRequestType {
    GET = 0, POST, HEAD, CONNECT, ERROR, ALLOK, E404 = 404
};

class HttpServer;
class HttpRequest;
class HttpClient;
class HttpClientRequest;
class HttpServerBlank;
class HttpException;

HttpRequest configureAnswer(const char *file);
HttpRequest configureRequest(const char *file);

class HttpServer{
    protected:
        char *root;
        char *error_path;

        SocketServer server;
        HttpRequest request;
    public:
        void setRoot(const char *root);
        void setErrorPath(const char *dir);
};

class HttpClient{

};
class HttpRequest{
    protected:
        char *method;
        char **fields; char **value;
        char *content;
    public:
        char *getValue(const char *value);
        void setValue(const char *value, const char *name);
        void setContent(const char *content);
        char* getContent();
        void setType(HttpRequestType rt);
        HttpRequestType getType();
        void fill(const char *src);
};

#endif
#define HTTP_LIB
#include <http.cpp>
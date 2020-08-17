//default paths
//потом сменю на более качественные настройки
#define HTTP_ERR_SZ 2048
#define HTTP_404_FN "404.html"
#define HTTP_404_EX(file) SocketException("localhost:server","Error file not found", (char*)file, int(HTTP_LIB), int((0x01a<<4)&404))
#define HTTP_IUK_EX SocketException("localhost:configure request", "Unknowen method", NULL, int(HTTP_LIB), int((0x01a<<4)&500))
#define HTTP_INF_EX SocketException("localhost:set value", "NULL fieldname of value", NULL, int(HTTP_LIB), int((0x01a<<4)&500))
#define HTTP_500_FN "$none"

#pragma once
#include <string.h>

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
class HttpServerBlank;
class HttpException;

class HttpRequest{
    //This class only contain and process data and requests.
    protected:
        char *method = NULL;
        char **fields = NULL; char **value = NULL;
        char *content = NULL;
    public:
        void operator=(HttpRequest req);

        char *getValue(const char *value);
        void setValue(const char *value, const char *name);
        void setContent(const char *content);
        char* getContent();
        void setType(HttpRequestType rt);
        HttpRequestType getType();
};

HttpRequest configureAnswer(const char *file);
HttpRequest configureRequest(const char *file);

class HttpServer{
    //This class work with socket and fill 
    protected:
        char *root;
        char *error_path;

        SocketServer *server;
        HttpRequest request;
    public:
    //Low-level and settings
        void setRoot(const char *root);
        void setErrorPath(const char *dir);
    //Network only
        void setup(SocketServer *server);
        void setup();
        void start();
};

class HttpClient{

};

#endif
#define HTTP_LIB
#include "./http.cpp"
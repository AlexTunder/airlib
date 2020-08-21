//default paths
//потом сменю на более качественные настройки
#define HTTP_ERR_SZ 2048
#define HTTP_404_FN "404.html"
#define HTTP_404_EX(file) SocketException("localhost:server","Error file not found", (char*)file, int(HTTP_LIB), int((0x01a<<4)&404))
#define HTTP_IUK_EX(additional) SocketException("localhost:configure request", additional, NULL, int(HTTP_LIB), int((0x01a<<4)&500))
#define HTTP_INF_EX(additional) SocketException("localhost:set value", additional, NULL, int(HTTP_LIB), int((0x01a<<4)&10))
#define HTTP_IRF_EX(additional) SocketException("localhost:invalid request", additional, NULL, int(HTTP_LIB), int((0x01a<<4)&51))
#define HTTP_500_FN "$none"

#pragma once
#include <string.h>
#include <stdio.h>
#ifndef SCFD
 #include "../Socket.hpp"
#endif
#ifndef HTTP_LIB
 #define HTTP_LIB 0x01a
 #ifndef HTTP_BITRATE
  #define HTTP_BITRATE 3072
 #endif
 #define HTTP_MAX_ARG 70
 #define HTTP 1.1

enum HttpRequestType {
    GET = 0, POST, HEAD, CONNECT, ALLOK = 200, E404 = 404
};

//Exceptions
class HttpException;
//Data storage and processing
class HttpRequest;

class HttpRequest{
    //This class only contain and process data and requests.
    protected:
        char *method = NULL;
        char **fields = new char*[HTTP_MAX_ARG]; char **value = new char*[HTTP_MAX_ARG];
        char *content = NULL;
    public:
        HttpRequest();

        void operator=(HttpRequest req);

        char *getValue(const char *value);
        void setValue(const char *value, const char *name);
        void setContent(const char *content);
        char* getContent();
        void setType(HttpRequestType rt);
        HttpRequestType getType();

        char *flush(char *to = NULL);
        void fill(const char *src);
};

HttpRequest configureAnswer(const char *file, const char *errpath, const char *root);
HttpRequest configureRequest(const char *file, HttpRequestType rt, char *additional = NULL);

class HttpServerHandler{
    //This class work with socket and fill 
    protected:
        char *root;
        char *error;

        char **routine_fn, **routine_vl;

        bool ready = 0;

    public:
        HttpServerHandler();
        HttpRequest requests, answer;
        void setRoot(const char *root);
        void setErrorPath(const char *dir);
        char *sendError(HttpRequestType rt);
        void setRoutineArg(const char *fieldname, const char *value);
        void removeRoutine(const char *fieldname);
        void fill(const char *src);
        bool isReady();
        char *flush();
};

class HttpClientHandler{
    protected:
        char **routine_fn, **routine_vl;

    public:
        HttpRequest requests, answer;
        void setRoutineArg(const char *fieldname, const char *value);
        void removeRoutine(const char *fieldname);
        HttpRequest get(const char *file);
        HttpRequest head(const char *file);
        HttpRequest post(const char *file, const char *content);
        HttpRequest connect(const char *host);
};

#endif
#include "./http.cpp"
//default paths
//потом сменю на более качественные настройки

#pragma once

#define HTTP_ERR_SZ 2048
#define HTTP_404_FN "404.html"
#define HTTP_404_EX(file) SocketException("localhost:server","Error file not found", (char*)file, int(HTTP_LIB), int((0x01a<<4)&404))
#define HTTP_IUK_EX(additional) SocketException("localhost:configure request", additional, NULL, int(HTTP_LIB), int((0x01a<<4)&500))
#define HTTP_INF_EX(additional) SocketException("localhost:set value", additional, NULL, int(HTTP_LIB), int((0x01a<<4)&10))
#define HTTP_IRF_EX(additional) SocketException("localhost:invalid request", additional, NULL, int(HTTP_LIB), int((0x01a<<4)&51))
#define HTTP_500_FN "$none"

#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include <stdio.h>

#ifndef SCFD
    class SocketException{
        public:
            SocketException(const char *address, const char *description, const char *additional, int port, int code){
                this->address = (char*)malloc(strlen(address));
                strcpy(this->address, address);
                this->description = (char*)malloc(strlen(description));
                strcpy(this->description, description);
                this->additional = (char*)malloc(strlen(additional));
                strcpy(this->additional, additional);
                this->port = port;
                this->codeOfError = code;
            }
            char *address, *description, *additional;
            int port, codeOfError;
            bool operator==(SocketException e);
            bool operator==(int codeOfException);
    };
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
class HttpHandler;

class HttpRequest{
    //This class only contain and process data and requests.
    protected:
        char *method = NULL;
        char **fields = new char*[HTTP_MAX_ARG]; char **value = new char*[HTTP_MAX_ARG];
        char *content = NULL;
    public:
        HttpRequest();

        void operator=(HttpRequest req);

        void clear(char what); //0 - nothing, 1 - args only, 2 - content only, 3 - content and args
        char *getValue(const char *value);
        void setValue(const char *value, const char *name);
        void setVirtual(const char *value, char *val);
        void setContent(const char *content);
        char* getContent();
        void setType(HttpRequestType rt);
        HttpRequestType getType();

        char *flush(char *to = NULL);
        void fill(const char *src);


};

HttpRequest configureAnswer(const char *file, const char *errpath, const char *root);
HttpRequest configureRequest(const char *file, HttpRequestType rt, char *additional = NULL);

#endif
#include "./http.cpp"
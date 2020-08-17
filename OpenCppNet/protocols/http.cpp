#pragma once

#ifndef HTTP_LIB
#include <./http.hpp>
#endif

HttpRequest configureAnswer(const char *file, char *errpath, char *root){
    HttpRequest sub;
    char *fn = new char[4096];
    strcpy(fn, root);
    strcat(fn, file);
    FILE *target = fopen(fn, "r");
    if(target == NULL){
        sub.setType((HttpRequestType)404);
        char errorpath[64] = "";
        strcpy(errorpath, errpath);
        strcat(errorpath, "/404.html");
        target = fopen(errorpath, "r");
        if(target == NULL){
            fclose(target);
            throw HTTP_404_EX(HTTP_500_FN);
        }
        else{
            char __fread_404[HTTP_ERR_SZ];
            fread(__fread_404, 1, HTTP_ERR_SZ, target);
            sub.setContent(__fread_404);
            sub.setType(HttpRequestType::E404);
            fclose(target);
            return sub;
        }
    }else{
        sub.setType(HttpRequestType::ALLOK);
        char __fread[HTTP_BITRATE];
        fread(__fread, 1, HTTP_BITRATE, target);
        sub.setContent(__fread);
        fclose(target);
        return sub;
    }
}
HttpRequest configureRequest(const char *file, HttpRequestType type){
    HttpRequest sub;
    sub.setType(type);
    sub.setValue("$target", (char*)file);
    return sub;
}

char *HttpRequest::getValue(const char *value){
    for(int i = 0; this->fields[i] != NULL ; i++){
        if(!strcmp(value, this->fields[i]))
            return this->value[i];
    }
    return NULL;
}
char *HttpRequest::getContent(){
    return this->content;
}
void HttpRequest::setContent(const char *content){
    this->content = new char[strlen(content)+1];
    strcpy(this->content, content);
}
void HttpRequest::setType(HttpRequestType type){
    this->method = new char[64];
    if(type == HttpRequestType::GET){
        strcpy(this->method, "GET $target HTTP/1.1");
    }else if(type == HttpRequestType::POST){
        strcpy(this->method, "POST $target HTTP/1.1");
    }else if(type == HttpRequestType::HEAD){
        strcpy(this->method, "HEAD $target HTTP/1.1");
    }else if(type == HttpRequestType::CONNECT){
        strcpy(this->method, "CONNECT $target HTTP/1.1");
    }else{
        sprintf(this->method, "HTTP/1.1 %i", (int)type);
    }
}
void HttpRequest::setValue(const char *fieldname, const char *value){
    int i = 0;
    for(; this->fields[i] != NULL; i++);
    this->fields[i] = new char[strlen(fieldname)];
    this->value[i] = new char[strlen(value)];
    strcpy(fields[i], fieldname);
    strcpy(this->value[i], value);
}
HttpRequestType HttpRequest::getType(){
    HttpRequestType rt;
    char *firstWordOfMethod, *tmp = new char[strlen(this->method)];
    strcpy(tmp, this->method);
    firstWordOfMethod = strtok(this->method, " ");
    if(!strcmp(firstWordOfMethod, "GET"))
        rt = HttpRequestType::GET;
    else if(!strcmp(firstWordOfMethod, "POST"))
        rt = HttpRequestType::POST;
    else if(!strcmp(firstWordOfMethod, "HEAD"))
        rt = HttpRequestType::HEAD;
    else if(!strcmp(firstWordOfMethod, "CONNECT"))
        rt = HttpRequestType::CONNECT;
    else{
        firstWordOfMethod = strtok(NULL, " ");
        rt = (HttpRequestType)atoi(firstWordOfMethod);
    }
    strcpy(this->method, tmp);
    return rt;
}
void HttpRequest::operator=(HttpRequest req){
    this->method = new char[strlen(req.method)+1];
    strcpy(this->method, req.method);
    if(req.fields != NULL && *req.fields != NULL){
        for(int i = 0; req.fields[i] != NULL; i++){
            this->fields[i] = new char(strlen(req.fields[i]));
            this->value[i] = new char(strlen(req.value[i]));
            strcpy(this->fields[i], req.fields[i]);
            strcpy(this->value[i], req.value[i]);
        }
    }
    this->content = new char[strlen(req.content)+1];
    strcpy(this->content, req.content);
}
HttpRequest::HttpRequest(){
    this->fields = new char *;
    this->value = new char *;
}
char *HttpRequest::flush(char *to){
    char *__sub;
    if(to == NULL)
        __sub= new char[HTTP_BITRATE];
    else
        __sub = to;
    sprintf(__sub, "%s", this->method);
    for(int i = 0; this->fields[i] != NULL; i++)
        sprintf(__sub, "%s\n%s: %s", __sub, this->fields[i], this->value[i]);
    sprintf(__sub, "%s\n\n%s", __sub, this->content);
    return __sub;
}

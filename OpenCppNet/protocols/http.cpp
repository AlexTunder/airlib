#pragma once

#ifndef HTTP_LIB
#include <./http.hpp>
#endif

HttpRequest configureAnswer(const char *file, char *errpath, char *root){
    HttpRequest sub;
    //debug
    // char a[16];
    // printf("%s",getcwd());
    FILE *target = fopen(file, "r");
    if(target == NULL){
        sub.setType((HttpRequestType)404);
        char errorpath[64] = "";
        strcpy(errorpath, errpath);
        strcat(errorpath, "/404.html");
        target = fopen(errorpath, "r");
        if(target == NULL)
            throw HTTP_404_EX(file);
        else{
            char __fread_404[HTTP_ERR_SZ];
            fread(__fread_404, 1, HTTP_ERR_SZ, target);
            sub.setContent(__fread_404);
            sub.setType(HttpRequestType::E404);
            free(__fread_404);
            fclose(target);
            return sub;
        }
    }else{
        sub.setType((HttpRequestType)200);
        char __fread[HTTP_BITRATE];
        fread(__fread, 1, HTTP_BITRATE, target);
        sub.setContent(__fread);
        sub.setType(HttpRequestType::ALLOK);\
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
    this->content = new char[strlen(content)];
    strcpy(this->content, content);
}
void HttpRequest::setType(HttpRequestType type){
    this->method = new char[32];
    if(type == HttpRequestType::GET){
        strcpy(this->method, "GET $target HTTP/1.1");
    }else if(type == HttpRequestType::POST){
        strcpy(this->method, "POST $target HTTP/1.1");
    }else if(type == HttpRequestType::HEAD){
        strcpy(this->method, "HEAD $target HTTP/1.1");
    }else if(type == HttpRequestType::CONNECT){
        strcpy(this->method, "CONNECT $target HTTP/1.1");
    }else{
        sprintf(this->method, "HTTP/1.1 %i");
    }
}
void HttpRequest::setValue(const char *fieldname, const char *value){
    if(fieldname == NULL || value == NULL) throw HTTP_INF_EX;
    int i = 0;
    for(i = 0; this->fields[i] != NULL; i++);
    this->fields[i] = new char[strlen(fieldname)];
    this->value[i] = new char[strlen(value)];
    strcpy(this->fields[i], fieldname);
    strcpy(this->value[i], value);
}
HttpRequestType HttpRequest::getType(){
    HttpRequestType rt;
    char *firstWordOfMethod;
    firstWordOfMethod = strtok(this->method, " ");
    if(!strcmp(firstWordOfMethod, "GET"))
        rt = HttpRequestType::GET;
    else if(!strcmp(firstWordOfMethod, "POST"))
        rt = HttpRequestType::POST;
    else if(!strcmp(firstWordOfMethod, "HEAD"))
        rt = HttpRequestType::HEAD;
    else if(!strcmp(firstWordOfMethod, "CONNECT"))
        rt = HttpRequestType::CONNECT;
    else rt = (HttpRequestType)atoi(firstWordOfMethod);
    return rt;
}
void HttpRequest::operator=(HttpRequest req){
    this->method = new char(strlen(req.method));
    strcpy(this->method, req.method);
    if(req.fields != NULL && *req.fields != NULL){
        for(int i = 0; req.fields[i] != NULL; i++){
            this->fields[i] = new char(strlen(req.fields[i]));
            this->value[i] = new char(strlen(req.value[i]));
            strcpy(this->fields[i], req.fields[i]);
            strcpy(this->value[i], req.value[i]);
        }
    }
    this->content = new char(strlen(req.content));
    strcpy(this->content, req.content);
}
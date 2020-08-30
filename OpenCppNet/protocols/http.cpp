#pragma once

#ifndef HTTP_LIB
 #include <./http.hpp>
 #include <sys/stat.h>
 #include <sys/types.h>
#endif
#include <string.h>

void slideFlag(char **arg, int from, int ignore){
    int _ign = 0;
    for(int i = from; _ign < ignore; i++){
        if(arg[i+1] == NULL) _ign++;
        arg[i] = arg[i+1];
    }
}
void cpfrag(char *to, char *from, int size){
    for(int i = 0; i < size - 1; i++)
        to[i] = from[i];
}

char *readWord(const char *from, const char *separator, int __n = 0){
    char *tmp = (char *)malloc(strlen(from)), *sub;
    strcpy(tmp, from);
    sub = strtok(tmp, separator);
    for(int i = 0; i < __n; i++){
        sub = strtok(NULL, separator);
    }
    return sub;
}

char *replace(const char *s, const char *old, const char *news){
    char *sub = new char[strlen(s) - strlen(old) + strlen(news)];
    char *_old = new char[strlen(old)];
    int counter = 0;

    strcpy(sub, "");
    for(int i = 0; s[i] != 0; i++){
        cpfrag(_old, (char*)&(s[i]), sizeof(old));
        if(!strcmp(_old, old)){
            sprintf(sub, "%s%s", sub, news);
            i += strlen(old) - 1;
        }else{
            sprintf(sub, "%s%c", sub, s[i]);
        }
    }
    return sub;
}


HttpRequest configureAnswer(const char *file, const char *errpath, const char *root){
    HttpRequest sub;
    char *fn = new char[4096];
    strcpy(fn, root);
    strcat(fn, file);
    FILE *target = fopen(fn, "r");
    if(target == NULL){
        sub.setType((HttpRequestType)404);
        char errorpath[128];
        sprintf(errorpath, "%s/404.html", errpath);
        target = fopen(errorpath, "r");
        if(target == NULL){
            sub.setContent("<html><head><title>HTTP 500 error</title></head><body>Server fail: no 404.html file</body></html>");
            sub.setType((HttpRequestType)(500));
            throw HTTP_404_EX(errpath);
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
/***************************** Setters *****************************/
void HttpRequest::setContent(const char *content){
    this->content = new char[strlen(content)+1];
    strcpy(this->content, content);
}
HttpRequest::HttpRequest(){
    for(int i = 0; i < HTTP_MAX_ARG; i++){
        this->fields[i] = NULL;
        this->value[i] = NULL;
    }
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
    if(!strcmp(fieldname, "$target")){
        if(value[0] != '/') throw HTTP_IRF_EX("You shuld to get files from /");
        this->method = replace(this->method, "$target", value);
    }
    
    int i = 0;
    for(; this->fields[i] != NULL; i++)
        if(!strcmp(this->fields[i], fieldname)) break;
    if(this->fields[i] == NULL){
        this->fields[i] = new char[strlen(fieldname)];
        this->value[i] = new char[strlen(value)];   
    }else{
        free(this->fields[i]);
        this->fields[i] = (char *)malloc(strlen(fieldname));
        free(this->value[i]);
        this->value[i] = (char *)malloc(strlen(value));
    }
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
    // strcpy()
    return rt;
}
void HttpRequest::operator=(HttpRequest req){
    this->method = new char[strlen(req.method)+1];
    strcpy(this->method, req.method);
    if(req.fields != NULL && *req.fields != NULL){
        for(int i = 0; req.fields[i] != NULL; i++){
            this->setValue((const char*)req.fields, (const char*)req.value);
        }
    }
    if(req.content != NULL){
        this->content = new char[strlen(req.content)+1];
        strcpy(this->content, req.content);
    }
}
char *HttpRequest::flush(char *to){
    char *__sub;
    if(to == NULL)
        __sub= new char[HTTP_BITRATE];
    else
        __sub = to;
    if(this->method == NULL || *this->method == 0) throw HTTP_IRF_EX("Method line is empty or NULL");
    sprintf(__sub, "%s", this->method);
    for(int i = 0; this->fields[i] != NULL; i++)
        sprintf(__sub, "%s\n%s: %s", __sub, this->fields[i], this->value[i]);
    if(this->content != NULL)
        sprintf(__sub, "%s\n\n%s", __sub, this->content);
    return __sub;
}
void HttpRequest::fill(const char *src){
    HttpRequestType rt;
    //Get 1st line
    char *tmp = new char[strlen(src)], *fl;
    strcpy(tmp, src);
    fl = strtok(tmp, "\n");
    this->method = new char[strlen(fl)];
    strcpy(this->method, fl);
    //Get type of request
    rt = this->getType();
    //If this is request, get target
    if(rt < HttpRequestType(100)){ //becouse all HTTP codes have 3 nums
        char *target = readWord(this->method, " ", 1);
        this->setValue("$target", target);
    }
    //Get other arguments...
    char *thisLine, *val, *fn;
    for(int i = 1; ;i++){
        thisLine = readWord(src, "\n\r", i);
        if(thisLine == NULL) break;
        fn = readWord(thisLine, ": ", 0);
        val = readWord(thisLine, " \n\r\0", 1);
        this->setValue(fn, val);
    }
}
HttpRequest configureRequest(const char *file, HttpRequestType rt, char *additional){
    HttpRequest sub;
    sub.setType(rt);
    sub.setValue("$target", file);
    if(rt == HttpRequestType::POST)
        if(additional != NULL)
            sub.setContent(additional);
        else
            throw HTTP_IRF_EX("Body of post is empty");
    return sub;
}
void HttpRequest::setVirtual(const char *fieldname, char *val){
    int i = 0;
    for(; this->fields[i] != NULL; i++)
        if(!strcmp(this->fields[i], fieldname)) break;
    if(this->fields[i] == NULL){
        this->fields[i] = new char[strlen(fieldname)];
        strcpy(this->fields[i], fieldname);
    }
    this->value[i] = val;
}
void HttpRequest::clear(char what){
    if(what == 1){
        for(int i = 0; this->fields[i] != NULL; i++){
            free(this->fields[i]);
            free(this->value[i]);
            this->value[i] = this->fields[i] = NULL;
        }
    }else if(what == 2){
        free(this->content);
        this->content = NULL;
    }else if(what == 3){
        free(this->content);
        this->content = NULL;
        for(int i = 0; this->fields[i] != NULL; i++){
            free(this->fields[i]);
            free(this->value[i]);
            this->value[i] = this->fields[i] = NULL;
        }
    }
}
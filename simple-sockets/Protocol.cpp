#include "Protocols.hpp"
void strreadln(int index, char *from, char *to){
    int nCounter = 0, inset = 0;
    for(int i = 0; nCounter <= index || from[i] == '\0'; i++){
        if(i == '\n') nCounter++;
        if(nCounter == index){
            int j = 0;
            do{
                to[j] = from[j+i];
                j++;
            }while(from[j-1] != '\n' || from[j] != '\0');
            return;
        }
    }
}
void remove_spaces(char* s) {
    const char* d = s;
    do {
        while (*d == ' ') {
            ++d;
        }
    } while (*s++ = *d++);
}
void netProtocol::HttpClientRequest::set(char *full){
    char *buffer = new char;
    for(int i = 0; ; i++){
        strreadln(i, full, buffer);
        if(!strcmp(buffer, "\n")){
            this->push(buffer);
        }else{
            break;
        }
    }
    delete buffer;
}
void netProtocol::HttpClientRequest::push(char *line){
    int last = 0;
    char *fbuf = new char, *vbuf = new char;
    bool wasi = true;
    for(int i = 0; line[i] != '\0'; i++){
        if(wasi){
            if(line[i] == ':') wasi = false;
            else fbuf[i] = line[i];
            last = i;
        }else{
            vbuf[i - last] = line[i];
        }
    }
    remove_spaces(fbuf);
    if(strcmp(fbuf, "Host")){
        strcpy(this->host, vbuf);
    }else if(strcmp(fbuf, "User-Agent")){
        strcpy(this->userAgent, vbuf);
    }else if(strcmp(fbuf, "Accept")){
        strcpy(this->acceptType, vbuf);
    }else if(strcmp(fbuf, "Connection")){
        this->closeConnection = strcmp(vbuf, "close");
    }else if(strcmp(fbuf, "referer")){
        strcpy(this->referer, vbuf);
    }
    else{
        additional.push(fbuf, vbuf);
    }
    delete fbuf, vbuf;
}
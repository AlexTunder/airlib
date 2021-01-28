// #include "../Socket.hpp"
#include <iostream>
#include "./http.hpp"

HttpRequest req;

int main(){
    char reqa[] = "GET / HTTP/1.1\n\
Connection: Keep-Alive\n\
New-val1: 1311\n";
    try{
        // req = configureAnswer("/protocols/test.cpp",\
        //     "/home/alexthunder/hentai/globus/OpenCppNet/protocols/err", "/home/alexthunder/hentai/globus/OpenCppNet");
        req.fill(reqa);
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError<<"\n\tError:"<<errno<<std::endl; //if exception happen, it's display all info
    }
    char *tmp;
    tmp = new char;
    tmp = req.getValue("$target");
    printf("Type: %i\n$target: %s\n", req.getType(), tmp);
}
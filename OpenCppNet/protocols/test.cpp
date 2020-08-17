#include "../Socket.hpp"
#include "./http.hpp"

HttpRequest req;

int main(){
    try{
        req = configureAnswer("/protocols/test.cpp",\
            "/home/alexthunder/hentai/globus/OpenCppNet/protocols/err", "/home/alexthunder/hentai/globus/OpenCppNet");
        req.setValue("Connection", "Keep-Alive");
        req.setValue("New-val1", "1311");
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError<<"\n\tError:"<<errno<<std::endl; //if exception happen, it's display all info
    }
    // printf("Type:\n\t%i\nArg:\n\t%s\nContent:\n%s\n", req.getType(), req.getValue("Connection"), req.getContent());
    char *ready = req.flush();
    printf("%s", ready);
    free(ready);
}
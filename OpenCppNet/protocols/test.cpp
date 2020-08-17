#include "../Socket.hpp"
#include "./http.hpp"

HttpRequest req;

int main(){
    try{
        req = configureAnswer("/home/alexthunder/hentai/globus/OpenCppNet/protocols/test.cpp", "err", "");
        printf("type:%s\ncontent:%s\n", req.getType(), req.getContent());
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError<<"\n\tError:"<<errno<<std::endl; //if exception happen, it's display all info
    }
}
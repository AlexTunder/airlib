#include "../Socket.hpp"
#include <iostream>

//At this time client is does not need for NetworkListener interface, but I always going to add interface for client. May be server part will be upgraded too.

SocketClient client; //declire client object

char *buffer = new char;//declire reading buffer

int main(){
    try{ //try to connect to server
        client.setBuffer(buffer); //setting reding buffer
        std::cout<<client.setBitrade(1024)<<std::endl; //set bitrade and check for value. It should be 1024
        client.connect("127.0.0.1", 8000); //connectiong to localhost in 8000 port
        client.send("hello!"); //sending message
        std::cout<<"Getted:"<<client.read()<<"\n"; //reading incoming data
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    return 0;
}

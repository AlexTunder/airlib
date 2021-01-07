#define __NOGDB
#ifndef __WIN32
#define __WIN32
#endif
#include "../Socket.hpp"
#include <iostream>

//At this time client is does not need for NetworkListener interface, but I always going to add interface for client. May be server part will be upgraded too.

SocketClient client; //declire client object

char *buffer = new char;//declire reading buffer

int main(){
    strcpy(buffer, "Error message: if you see that, socket is dousn't work.\0");
    printf("Testing:\n\tLevel: prod.\n\tprotocol: TCP/IP\n\thost: google.com\n\tTesting status: in process\n=================\n");
    try{ //try to connect to server
        client.setBuffer(buffer); //setting reding buffer
        std::cout<<"Bitrate setting up: " << client.setBitrate(1024)<<std::endl; //set bitrade and check for value. It should be 1024
        client.connect("127.0.0.1", 3000);
        client.send("hello!"); //sending message
        client.read();
        std::cout<<"Getted:"<<buffer<<"\n"; //reading incoming data
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError<<"\n\tAdditional code:" << e.additionalCode << "\n"; //if exception happen, it's display all info
        return -1;
    }
    printf("Result (incoming data): %s", buffer);
    return 0;
}

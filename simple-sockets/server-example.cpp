#include "./Socket.hpp" //include library

SocketServer server; //making server object

void *listener(void *rawPointerToArguments){ //making listener function for communicate with clients
    netServerAttribures *pointerToArgument = (netServerAttribures*)rawPointerToArguments; //decoding arguments for this listener.
    std::cout <<"read:\t"<< pointerToArgument->socketServer->read() << std::endl; //reading incoming message and print it
    while(1){ //endless loop
            pointerToArgument->socketServer->send("Hello!", pointerToArgument->connectionID); //sending "hello" message
            pointerToArgument->socketServer->read(pointerToArgument->connectionID); //reading from current connection
            std::cout<<pointerToArgument->socketServer->buffer<<"\n"; //print message from buffer
    }
}

char readingBuffer[1024]; //reading buffer

int main(){
    try{ //try to start server
        std::cout<<server.setBitrade(1024)<<std::endl; //set a bitrade, print current bitrade (shoud be 1024)
        server.setBuffer(readingBuffer); //setting buffer
        server.bind("127.0.0.1", 8000); //binding server in local host and 8000 port
        server.start(listener, 5); //start listening for clients. MAx. count of clients: 5
        while(1){
            //endless loop. If you skip this programm will be finished with 0 code, becouse listen thread detauchet from main.
        }
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    return 0;
}
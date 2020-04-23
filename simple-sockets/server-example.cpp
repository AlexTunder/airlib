#include "./Socket.hpp" //include library

SocketServer server; //making server object

void *listener(void *rawPointerToArguments){ //making listener function for communicate with clients
    throwNetAttr *pointerToArgument = (throwNetAttr*)rawPointerToArguments; //decoding arguments for this listener.
    try{
        sleep(1);
        pointerToArgument->ptsd->sockPtr->setBuffer(server.buffer);
        // std::cout <<"read:\t"<< pointerToArgument->ptsd->sockPtr->read(pointerToArgument->connID) << std::endl; //reading incoming message and print it
        while(1){ //endless loop
            pointerToArgument->ptsd->socketServer->read(pointerToArgument->connID); //reading from current connection
            pointerToArgument->ptsd->socketServer->send("Hello!", pointerToArgument->connID); //sending "hello" message
            std::cout<<pointerToArgument->ptsd->socketServer->buffer<<"\n"; //print message from buffer
        }
    }catch(SocketException e){ //if exception occuped
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
        if (e.codeOfError == SOCKEXC_DISCONN){ //if exception say about client disconnectiong
            pointerToArgument->ptsd->socketServer->cc--; //-1 from current connections
            sideTo(server.connection, pointerToArgument->connID, pointerToArgument->ptsd->socketServer->cc+1); //slide list with connections
        }
    }
}

char readingBuffer[1024]; //reading buffer

// NetworkListener networkListener;

int main(){
    signal(SIGPIPE, SIG_IGN);
    try{ //try to start server
        // networkListener.SetListener(DEFAULT_LISTENER,*[](void *arg){ // this is set default requests listener to lambda-function. You also can you pointer to func.
        //     networkListener.read(arg); // read data from current connection
        //     networkListener.send(arg, "hello!"); // send data to current connection
        // });
        std::cout<<server.setBitrade(1024)<<std::endl; //set a bitrade, print current bitrade (shoud be 1024)
        server.setBuffer(readingBuffer); //setting buffer
        server.bind("127.0.0.1", 8000); //binding server in local host and 8000 port
        server.start(listener, 5); //start listening for clients. MAx. count of clients: 5
        while(1){
            printf("current connections: %i\n", server.cc);
            //endless loop. If you skip this programm will be finished with 0 code, becouse listen thread detauchet from main.
        }
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    return 0;
}
#define __NOGDB
#include "./Socket.hpp" //include library
#include "protocols/http.hpp"

SocketServer server; //making server object

NetworkListener listener; //declire connection listener

//You also can use pointer to function like: void*(void*), so I should to write this below
//I not recomend this method for setting listener. NetworkClass work better and it's have nice interface
//for using pointer-to-function listener you have to know at least basic architecture of library
//In conclusion: USE CLASS INTERFACE!!!

char readingBuffer[1024]; //reading buffer

int main(){
    for(int i = 0; i<1024; i++)
        readingBuffer[i] = 0;
    try{
        listener.setListener(UPLOADS_LISTENER,[](void *attr){
            ListenerStream <int>stream(attr); //Declare stream with connection
            try{ // try to do next:
                printf("[%i] new connection: %i", stream.getCurrent(), stream.getCurrent());
                stream.setBuffer(readingBuffer); //istall buffer for this stream
                while (1){ //endless loop: this is active, when connection exist
                    std::cout<<stream.read()<<std::endl; //reading data
                }
            }catch(SocketException e){ //if exception occuped
                std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
                if (e.codeOfError == SOCKEXC_DISCONN || e.codeOfError == SOCKEXC_BROKENP){ //if exception say about client disconnectiong
                    stream.close(); //close current stream & connection
                }
            }
        });
        listener.setListener(DOWNLOAD_LISTENER, [](void *raw){
            ListenerStream <int>stream(raw); //Declare stream with connection
            try{
                stream.setBuffer(readingBuffer);
                while(1){
                    if(readingBuffer[0] != '\0'){
                        stream.send("Hello world!");
                    }
                }
            }catch (SocketException e){
                std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
                if (e.codeOfError == SOCKEXC_DISCONN || e.codeOfError == SOCKEXC_BROKENP){ //if exception say about client disconnectiong
                    stream.close(); //close current stream & connection
                }
            }
        });
        //try to start server
        std::cout<<server.setBitrade(1024)<<std::endl; //set a bitrade, print current bitrade (shoud be 1024)
        server.setBuffer(readingBuffer); //setting buffer
        server.bind("127.0.0.1", 8000); //binding server in local host and 8000 port
        server.start(&listener, 5); //start listening for clients. MAx. count of clients: 5
        while(1){
            // printf("current connections: %i\n", server.cc);
            //endless loop. If you skip this programm will be finished with 0 code, becouse listen thread detauchet from main.
        }
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\tAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    return 0;
}

#include "../Socket.hpp" //include library

SocketServer server; //making server object

NetworkListener listener; //declire connection listener

char readingBuffer[1024]; //reading buffer

int main(){
    try{
        listener.setListener(DEFAULT_LISTENER,[](void *attr){
            ListenerStream <int>stream(attr); //Declare stream with connection
            stream.setBuffer(readingBuffer); //istall buffer for this stream
            try{ // try to do next:
                printf("[%i] open\n", stream.getCurrent());
                while (1){ //endless loop: this is active, when connection exist
                    stream.read(); //reading datas
                    std::cout<<std::string(readingBuffer)<<std::endl; //display buffer
                    stream.send("ALLOK"); //send ALLOK string
                }
            }catch(SocketException e){ //if exception occuped
                std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
                if (e.codeOfError == SOCKEXC_DISCONN || e.codeOfError == SOCKEXC_BROKENP){ //if exception say about client disconnectiong
                    stream.close(); //close current stream & connection
                    printf("[%i] closed\n", stream.getCurrent());
                }
            }
        });
    }catch (SocketException e){
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\tAdditional info:"<<int(*e.additional)<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    try{ //try to start server
        std::cout<<server.setBitrate(1024)<<std::endl; //set a bitrade, print current bitrade (shoud be 1024)
        server.setBuffer(readingBuffer); //setting buffer
        server.bind("127.0.0.1", 8000); //binding server in local host and 8000 port
        server.start(&listener, 5); //start listening for clients. MAx. count of clients: 5
        printf("ALLOK");
        while(1){
            // printf("current connections: %i\n", server.cc);
            //endless loop. If you skip this programm will be finished with 0 code, becouse listen thread detauchet from main.
        }
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\tAdditional info:"<<int(*e.additional)<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    return 0;
}
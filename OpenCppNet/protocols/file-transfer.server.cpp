#include "../Socket.hpp" //include library
#include <sys/stat.h>

SocketServer server; //making server object

NetworkListener listener; //declire connection listener

char readingBuffer[1024], *last = new char[100]; //reading buffer

char **requst = new char*;

struct stat *buff = (struct stat*)malloc(sizeof(struct stat));

class FNODE{
    public:
        int fill(char *path, int blockSize = 512);
        int blocks;
        int blocksize;
        int size;
        char *filename;
};

void shell(){
    std::string command;
    std::cin>>command;
    if(command == "list"){
        std::cin>>command;
        if(command == "$connection-all"){
            printf("Server current connection state:\n");
            printf("Connection\tAddress\t\tValue\tCVS\tThread\n");
            for(int i = 0; i < server.cc; i++){
                printf("%i\t\t%p\t%i\t%i\t%p\n", i, server.connection[i], *server.connection[i], server.cc, server.threadStorage);
            }
        }else{
            printf("%s - is unknowen listing argument\n", command.c_str());
        }
    }else if(command == "kill"){
        std::cin>>command;
        if(server.threadStorage[atoi(command.c_str())] != NULL && server.connection[atoi(command.c_str())]){
            delete(server.connection[atoi(command.c_str())]);
            delete(server.threadStorage[atoi(command.c_str())]);
            server.threadStorage[atoi(command.c_str())] = NULL;
            server.connection[atoi(command.c_str())] = NULL;
            server.cc--;
        }
    } else if(command == "exit"){
        if(server.cc == 0)
            exit(0);
        else{
            printf("Are you sure? (y/N): ");
            char c;
            std::cin >> c;
            if(c == 'y' || c == 'Y')
                exit(0);
        }
    }
    else{
        printf("%s - is unknowen command\n", command.c_str());
    }
}

bool capia = 0;

int main(){
    listener.SetListener(DEFAULT_LISTENER,[](void *attr){
        FNODE **nodes = new FNODE*;
        ListenerStream stream(attr); //Declare stream with connection
        printf("[info(%i)] Connected new.\n", stream.getCurrent());
        stream.setBuffer(readingBuffer); //istall buffer for this stream
        try{ // try to do next:
            stream.read();
            int subLines = -1, __sub = 0;
            requst[0] = (char*)calloc(128,1);
            // subLines = strsplit(readingBuffer, " ", requst);
            requst[0] = strtok_r(readingBuffer, " ", &last);
            for(int i = 1; requst[i-1] != NULL; i++){
                requst[i] = strtok_r(NULL, " ", &last);
                subLines = i-1;
            }
            if(subLines < 1){
                printf("[error(%i)] Client send invalid request.\n", stream.getCurrent());
                stream.send("ERROR CLIENT REQUEST FORMAT");
                stream.close();
            }else{
                if(!strcmp(requst[0], "FILE")){
                    nodes[0] = (FNODE*)malloc(sizeof(FNODE));
                    if(int a = (nodes[0]->fill(requst[1]),512)==-1){
                        switch(errno){
                            case EACCES:
                                sprintf(last, "reason: No access.");
                                break;
                            case EIO:
                                sprintf(last, "reason: Error in OS FS reading.");
                                break;
                            case ELOOP:
                                sprintf(last, "reason: Looped.");
                                break;
                            case ENAMETOOLONG:
                                sprintf(last, "reason: Name too long.");
                                break;
                            case ENOENT:
                                sprintf(last, "reason: A component of path does not name an existing file or path is an empty string..");
                                break;
                            case ENOTDIR:
                                sprintf(last, "reason: A component of the path prefix is not a directory.");
                                break;
                            case EOVERFLOW:
                                sprintf(last, "reason: A value to be stored would overflow one of the members of the stat structure.");
                                break;
                            default:
                                sprintf(last, "reason: unknowen(%i)", a);
                                break;
                        }
                        printf("[error(%i)] File (%s) not found or failed to get stat(), %s\n", stream.getCurrent(), nodes[0]->filename, last);
                        stream.send(std::string("ERROR SERVER TARGET BUSY\n")+readingBuffer);
                        stream.close();
                        printf("[info(%i)] connection closed.\n");
                    }else{
                        printf("[info(%i)] FNODE for (%s) loaded. Size: %i, blocks: %i, blocksize: %i\n", stream.getCurrent(), nodes[0]->filename, nodes[0]->size, nodes[0]->blocks, nodes[0]->blocksize);
                        sprintf(readingBuffer, "FNODES %i BLOCKSIZE %i", nodes[0]->blocks, nodes[0]->blocksize);
                        printf("[info(%i)] answer configured: %s\n", stream.getCurrent(), readingBuffer);
                        stream.send(readingBuffer);
                        strcpy(readingBuffer, "");
                        FILE *target = fopen(nodes[0]->filename,"r");
                        if(target == NULL){
                            sprintf(readingBuffer, "[error(%i)] target file (%s) is NULL\n", stream.getCurrent(), requst[1]);
                            switch(errno){
                                case EACCES:
                                    sprintf(readingBuffer, "reason: No access.");
                                    break;
                                case EIO:
                                    sprintf(readingBuffer, "reason: Error in OS FS reading.");
                                    break;
                                case ELOOP:
                                    sprintf(readingBuffer, "reason: Looped.");
                                    break;
                                case ENAMETOOLONG:
                                    sprintf(readingBuffer, "reason: Name too long.");
                                    break;
                                case ENOENT:
                                    sprintf(readingBuffer, "reason: A component of path does not name an existing file or path is an empty string..");
                                    break;
                                case ENOTDIR:
                                    sprintf(readingBuffer, "reason: A component of the path prefix is not a directory.");
                                    break;
                                case EOVERFLOW:
                                    sprintf(readingBuffer, "reason: A value to be stored would overflow one of the members of the stat structure.");
                                    break;
                                default:
                                    sprintf(readingBuffer, "reason: unknowen(%i)", a);
                                    break;
                            }
                            printf("%s\n", readingBuffer);
                            stream.send("ERROR SERVER FILE NULL");
                            stream.close();
                        }else{
                            for(int i = 0; i<nodes[0]->blocks; i++){
                                strcpy(readingBuffer, "");
                                fread(readingBuffer, 1, nodes[0]->blocksize, target);
                                stream.send(readingBuffer);
                                strcpy(readingBuffer, "");
                                stream.read();
                                if(strcmp(readingBuffer, "FNODE ACCEPTED")){
                                    printf("[info(%i)] Client miss handshake or MANU-command.\n", stream.getCurrent());
                                    stream.send("ERROR CLIENT MISS HANDSHAKE");
                                    stream.close();
                                }else{
                                    printf("[info(%i)] Client accpet FNODE %i/%i.\n", stream.getCurrent(), i+1, nodes[0]->blocks);
                                }
                            }
                            printf("[info(%i)] Session done.\n", stream.getCurrent());
                            stream.send("SESSION DONE");
                            stream.close();
                        }
                    }
                }else{
                    printf("[warning(%i)] Special indentifier: %s\n", stream.getCurrent(), requst[0]);
                    for(int i = 0; i < subLines; i++)
                        printf("\t[warning-info(%i)] Spetial request index(%i) str(%s)\n", stream.getCurrent(), i, requst[i]);
                }
            }
        }catch(SocketException e){ //if exception occuped
            std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\taDditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
            stream.close(); //close current stream & connection
        }
    });
    try{ //try to start server
        char *tmp = new char;
        getcwd(tmp, sizeof(tmp));
        printf("[info(:-1)] Server was binded. Work path: %s, full: %s\n", tmp, getenv("PWD"));
        delete tmp;
        server.setBitrade(1024); //set a bitrade, print current bitrade (shoud be 1024)
        server.setBuffer(readingBuffer); //setting buffer
        server.bind("127.0.0.1", 10512); //binding server in local host and 8000 port
        server.start(&listener, 5); //start listening for clients. MAx. count of clients: 5
        while(1){
            shell();
            // printf("current connections: %i\n", server.cc);
            //endless loop. If you skip this programm will be finished with 0 code, becouse listen thread detauchet from main.
        }
    }catch(SocketException e){ //if exception happen
        std::cout<<e.description<<"\n\tAddress:"<<e.address<<"\n\tPort:"<<e.port<<"\n\tAdditional info:"<<e.additional<<"\n\tCode of error:"<<e.codeOfError; //if exception happen, it's display all info
    }
    return 0;
}

int FNODE::fill(char *path, int blocksize){
    this->filename = (char*)calloc(strlen(path),1);
    strcpy(this->filename, path);
    this->blocksize = blocksize;
    int s = stat(path, buff);
    if(s != -1){
        this->size = buff->st_size;
        if(this->size % blocksize == 0) this->blocks = int(this->size/blocksize);
        else this->blocks=int(this->size/blocksize)+1;
    }
    return s;
}
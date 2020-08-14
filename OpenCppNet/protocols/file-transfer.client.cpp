#include "../Socket.hpp"

SocketClient client;

const char fmode[] = "wb";

char buffer[1024], *last = new char[1024];
char **subreq = new char*;

FILE *result;

int main(int argc, char **argv){
    if(argc < 1)
        return 1;
    try{
        client.setBitrade(1024);
        client.setBuffer(buffer);
        client.connect("127.0.0.1", 10512);
        sprintf(buffer, "FILE %s", argv[1]);
        if(argc == 1)
            result = fopen(argv[1], fmode);
        else
            result = fopen(argv[2], fmode);
        client.send(buffer);
        strcpy(buffer, "");
        client.read();

        int currentFNODE = 0, allNodes = 0, nodeSize = 0;
        int subline = 0, __sub = 0;
        
        subreq[0] = strtok_r(buffer, " ", &last);
        for(int i = 1; subreq[i-1] != NULL; i++){
            subreq[i] = strtok_r(NULL, " ", &last);
            subline = i-1;
        }

        for(int i = 0; i < subline; i++){
            if(!strcmp(subreq[i], "FNODES")){
                allNodes = atoi(subreq[i + 1]);
                i++;
            }else if(!strcmp(subreq[i], "BLOCKSIZE")){
                nodeSize = atoi(subreq[i+1]);
                i++;
            }
        }

        while(1){
            client.send("FNODE ACCEPTED");
            client.read();
            if(!strcmp(buffer, "SESSION DONE"))
                break;
            fputs(buffer, result);
            printf("%i/%i fnodes loaded.\n", currentFNODE+1, allNodes);
            currentFNODE++;
        }
        printf("Loading %s :=>: %s completed.\n", argv[1], argv[2]);
    }catch(SocketException e){
        printf("Exc");
    }
    return 0;
}
#pragma once
#ifndef SCFD
#define SCFD 1
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <stdio.h>
#include <string.h> 
#include <string>
#include <sys/ioctl.h>
#include <net/if.h>
#include <iostream>
#include <pthread.h>

class Socket{
    public:
        /** Comment this later **/
        Socket();
        void *buffer;
        int bitrade;
        struct sockaddr_in address; 
        int fd, **connection, opt, cc;
        size_t al = sizeof(address);
        /** Communicate with connection **/
        std::string read();
        std::string read(int conn);
        int send(std::string data);
        int send(std::string data, int conn);
        /** Connection managment; **/
        int kill(int conn);
        int close(int conn);
        int addConnection();
        int setActive(int conn);
        int setBitrade(int bitrade);
        char *setBuffer(char *buffer);
};

class SocketException{
    public:
        SocketException(char *address, char *description, char *additional, int port, int code);
        char *address, *description, *additional;
        int port, codeOfError;
};

class SocketServer : public Socket{
    public:
        struct throwenSocketServerStruct{
            SocketServer *socketServer;
            void *(*listener)(void *arg);
            int connectionID;
        };
        /*** I should to use this structure, becouse it's have dependes ***/
        int bind(std::string addr, int port, int protocol = SOCK_STREAM);
        void start(void *(*listener)(void *arg), int maxClients);
        struct throwenSocketServerStruct arg;
        static void *listen(void *arg);
        pthread_t **threadStorage;
        int port; std::string me;
        pthread_t mainThread;
        pthread_attr_t attr;
        bool state = true;
        SocketServer();
};



class SocketClient : public Socket{
    public:
        SocketClient();
        int connect(std::string addr, int port, int protocol = SOCK_STREAM);
};

class Coder{
    private:
        char *src, *out;
    public:
        void (*algo)(char*, char*);
};

class Encoder : public Coder{
    public:
        char *operator()(char *src, char *key);
};

class Decoder : public Coder{
    public:
        char *operator()(char *src, char *key);
};

void anonimusEncode(char*, char*);
void anonimusDecode(char*, char*);

#define netServerAttribures SocketServer::throwenSocketServerStruct
#include "Socket.cpp"
#endif
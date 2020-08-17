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
#include <signal.h>
//Listeners
#define DEFAULT_LISTENER        0
#define UPLOADS_LISTENER        1
//Exception codes
#define SOCKEXC_DISCONN         0x71
#define SOCKEXC_BROKENP         0x72
#define SOCKEXC_TIMEOUT         0x73

class Socket{
    public:
        /** Comment this later **/
        Socket();
        char *buffer;
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

class ___SockPTR___uniQED__{
    public:
        ___SockPTR___uniQED__(void *client, bool isServer);
        int *fd;
        int **conn;
        int *cc;
        int *bitrade;
        char *buffer;
        /** Communicate with connection **/
        std::string read();
        std::string read(int conn);
        int send(std::string data);
        int send(std::string data, int conn);
        /** Connection managment; **/
        int kill(int conn);
        int close(int conn);
        int setBitrade(int bitrade);
        char *setBuffer(char *buffer);
        int connn;
};

class SocketException{
    public:
        SocketException(char *address, char *description, char *additional, int port, int code);
        char *address, *description, *additional;
        int port, codeOfError;
        bool operator==(SocketException e);
        bool operator==(int codeOfException);
};

SocketException SockTimeOut("", "Time for connection is out.", "", 0, SOCKEXC_TIMEOUT);

class NetworkListener;

class SocketServer : public Socket{
    //Да и вообще здесь много говна, надо поменять
    public:
        //Надо переписать! Но не срочно, пока работает. Но вообще - сменить всю эту хню...
        // ...на тупо 2 поинтера - клиент и сервер. И мозги ипать не надо, ___sock_ptr_ пропадет
        // ... и с этим уйдет и говнокод, но ListenerStream надо оставить, ибо негоже в листенере...
        // ... давать сразу поинтер на сам сервер или кллиент.
        struct throwenSocketServerStruct{
            SocketServer *socketServer = nullptr;
            void *(*listener)(void *arg);
            int connectionID;
            ___SockPTR___uniQED__ *sockPtr = nullptr;
        };
        /*** Yet I should to use this structure, becouse it's have dependes ***/
        int bind(std::string addr, int port, int protocol = SOCK_STREAM);
        void start(void *(*listener)(void *arg), int maxClients);
        void start(NetworkListener *listener, int maxClients);
        struct throwenSocketServerStruct arg;
        static void *listen(void *arg);
        pthread_t **threadStorage;
        NetworkListener *listener; //Стоило бы убрать и сделать что то другое
        int port; std::string me;
        pthread_t mainThread;
        pthread_attr_t attr;
        SocketServer();
        int state = 0;
        pthread_mutex_t mutex;
        pthread_mutexattr_t mutexattr;
};

class NetworkListener{
    private:
        struct listenersStruct{
            ___SockPTR___uniQED__ *arg;
            void (*downstream)(void *arg); //If double-stream enable use as download stream and write data to buffer
            void (*upstream)(void *arg); //If double-stream enable use as upload stream and send data from buffer
            void (*defaultListener)(void *arg);
        }listeners;
        void *(*mainListener)(void *parg);
    public:
        static void *threadable(void *arg);
        void SetListener(int type, void (*listener)(void *parg));
        void Set___SockPTR___uniQED__(___SockPTR___uniQED__ *sockptr);
        
        //Work like low-level read() and send(), but have timeout (default = 1000ms)
        //If time out throw SocketException (TimeOutException)
        void sendRequest(const char *data);
        char *readRequest();
};

class SocketClient : public Socket{
    public:
        SocketClient();
        int connect(std::string addr, int port, int protocol = SOCK_STREAM);
        int read();
};

class throwNetAttr{
    public:
        SocketServer::throwenSocketServerStruct *ptsd;
        int connID;
        NetworkListener *listener;
        throwNetAttr(SocketServer::throwenSocketServerStruct *netData, int connID); 
        throwNetAttr(SocketServer::throwenSocketServerStruct *attr, int conn, NetworkListener *listener);
};

class ListenerStream{
    private:
        throwNetAttr *attributes;
        char *buffer;
    public:
        void close();
        int getCurrent();
        std::string read();
        ListenerStream(void *arg);
        void send(std::string data);
        void setBuffer(char *buffer);
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

___SockPTR___uniQED__* getSockPointerFromAttr(SocketServer::throwenSocketServerStruct *attr);

#define netServerAttribures SocketServer::throwenSocketServerStruct
#define NetworkAttributes ___SockPTR___uniQED__ 
#include "Socket.cpp"
#endif
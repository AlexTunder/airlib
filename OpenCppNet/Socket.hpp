#pragma once
#ifndef SCFD
#define SCFD "0.0.8"
#ifdef __WIN32
 #define WIN32_LEAN_AND_MEAN
 #pragma comment(lib, "ws2_32.lib")
 #include <winsock.h>
 #include <ws2tcpip.h>
 #include <ws2tcpip.h>
#else
 #include <sys/types.h>
 #include <sys/socket.h> 
 #include <arpa/inet.h>
 #include <sys/ioctl.h>
 #include <net/if.h>
#endif
#ifdef __WIN32
 #ifndef USE_PTHREAD
  #include <windows.h>
 #else
  #ifndef __WIN32_pthread
   #error Windows dousen't support arch for pthread or pthread.h is not included
  #elif __WIN32
   #warning POSIX thread is ported for windows, so some functions can works wrong
   #include <pthread.h>
  #endif
 #endif
#else
 #define USE_PTHREAD
 #include <pthread.h>
#endif
#include <stdio.h>
#include <unistd.h> 
#include <stdio.h>
#include <string.h> 
#include <string>
#include <iostream>
#include <vector>
//Global Directives
#ifdef __WIN32
 //Directives only for MS Windows
 #ifndef __WIN32_pthread
  #define LAMBDA_THREAD_END DWORD       //it's mean, what return code for thread is DWORD (aka long(32))
 #else
  #define LAMBDA_THREAD_END void*
 #endif
 #define THREADING_ARCH HANDLE
#else
 //Only for Unix-like (or other POSIX-based systems)
 #define LAMBDA_THREAD_END void*
 #define THREADING_ARCH pthread_t
#endif
//Listeners
#define DEFAULT_LISTENER        0x00 //Echo   Network Thread
#define UPLOADS_LISTENER        0x01 //First  Network Thread
#define DOWNLOAD_LISTENER       0x02 //Second Network Thread
//Exception codes
// 1X - Any side, runtime error or warning (user's algorithm call some exceptions)
// 2X - Any side, settings or using error
// 3X - Any side, Iternel errors (architecture fail)
// 4X - Any side, Called from outside
// 5X - Any side, Preparing stage fail
// 7X - Any side, while connection is active
// 8X - Server fail
// 9X - Client fail
// Additional codes:
// 
#define SOCKEXC_DISCONN         0x70 //Connection refused: closed from other side
#define SOCKEXC_BROKENP         0x71 //Connection refused: closed or broken
#define SOCKEXC_TIMEOUT         0x72 //Connection refused: time out
#define SOCKEXC_ALINUSE         0x80 //Failed to bind server: already in use
#define SOCKEXC_ARCHUNS         0x50 //OS arch is unsupported
#define SOCKEXC_UNCERTL         0x20 //Wrong listener list: one or more listeners is conflicting
#define SOCKEXC_ITERNAL         0x30 //Iternal error excite exception. in additional info taken struct with all info
#define SOCKEXC_ARCHERR         0x31 //OS unique error
#define SOCKEXC_RESOLVE         0x91 //Connection denied: host not found or not resolved
#define SOCKEXC_NOFILED         0x32 //No file description for socket
//Listeners return codes
#define endStream(x)        return(LAMBDA_THREAD_END(x))// type endStream(exit code); for break listener's loop
#define thread_routine_t    LAMBDA_THREAD_END           // type of poiner to function for thread
#define thread_t            THREADING_ARCH              // type of thread

class Socket{
    public:
        //NT-only declire
        #ifdef __WIN32
         WSADATA wsa;
        #endif
        /** Comment this later **/
        Socket();
        char *buffer;
        int bitrade;
        struct sockaddr_in address; 
        int fd, cc;
        std::vector <int*> connection;
        char opt;
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
        std::vector <int*> conn;
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
        SocketException(const char *address, const char *description, const char *additional, int port, int code, int additionalCode = -1);
        char *address, *description, *additional;
        int port, codeOfError, additionalCode;
        bool operator==(SocketException e);
        bool operator==(int codeOfException);

        void print(); //default print if exception occupted
        void call(); //same as print() but stop programm after finish
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
            thread_routine_t (*listener)(void *arg);
            int connectionID;
            ___SockPTR___uniQED__ *sockPtr = nullptr;
        };
        /*** Yet I should to use this structure, becouse it's have dependes ***/
        int bind(std::string addr, int port, int protocol = SOCK_STREAM, long additionalFlag = SO_REUSEADDR);
        void start(thread_routine_t (*listener)(void *arg), int maxClients);
        void start(NetworkListener *listener, int maxClients);
        struct throwenSocketServerStruct arg;
        static thread_routine_t listen(void *arg);
        std::vector <thread_t *> threadStorage;
        thread_t mainThread;
        #ifdef USE_PTHREAD
         pthread_attr_t attr;
         pthread_mutex_t mutex;
         pthread_mutexattr_t mutexattr;
        #endif
        NetworkListener *listener; //Стоило бы убрать и сделать что то другое
        int port; std::string me;
        SocketServer();
};

// template <typename tn>
class NetworkListener{
    protected:
        struct listenersStruct{
            ___SockPTR___uniQED__ *arg;
            void (*downstream)(void *arg) = NULL; //If double-stream enable use as download stream and write data to buffer
            void (*upstream)(void *arg) = NULL; //If double-stream enable use as upload stream and send data from buffer
            void (*defaultListener)(void *arg) = NULL;
        }listeners;
        void *(*mainListener)(void *parg);
    public:
        int timeout;
        bool isMonolite();
        static void *threadable(void *arg);
        static void *Upthreadable(void *arg);
        static void *Downthreadable(void *arg);
        void setListener(int type, void (*listener)(void *parg));
        void Set___SockPTR___uniQED__(___SockPTR___uniQED__ *sockptr);
        
        //Work like low-level read() and send(), but have timeout (default = 1000ms)
        //If time out throw SocketException (TimeOutException)
        //IT's will work in 0.0.9
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
        void *mutualRes = NULL;
        SocketServer::throwenSocketServerStruct *ptsd;
        int connID;
        NetworkListener *listener;
        throwNetAttr(SocketServer::throwenSocketServerStruct *netData, int connID); 
        throwNetAttr(SocketServer::throwenSocketServerStruct *attr, int conn, NetworkListener *listener);
};

template <typename userInfo>
class ListenerStream{
    protected:
        throwNetAttr *attributes;
        char *buffer;
    public:
        void close();
        int getCurrent();
        void setMutual(userInfo info){
            if(this->attributes->mutualRes == NULL) this->attributes->mutualRes = malloc(sizeof(userInfo));
            *((userInfo*)(this->attributes->mutualRes)) = info;
        }
        void setMutual(userInfo *info){
            this->attributes->mutualRes = info;
        }
        std::string read();
        userInfo *mutualRes(){
            return (userInfo*)(this->attributes->mutualRes);
        }
        ListenerStream(void *arg);
        void send(std::string data);
        void setBuffer(char *buffer);
};

void anonimusEncode(char*, char*);
void anonimusDecode(char*, char*);

___SockPTR___uniQED__* getSockPointerFromAttr(SocketServer::throwenSocketServerStruct *attr);

#define netServerAttribures SocketServer::throwenSocketServerStruct
#define NetworkAttributes ___SockPTR___uniQED__ \
 #ifndef SCFD_bin
  #include "Socket.cpp"
#endif
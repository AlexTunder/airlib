#pragma once
#ifndef SCFD
#define SCFD "0.0.8"
#ifdef __WIN32
 #define WIN32_LEAN_AND_MEAN
 #pragma comment(lib, "ws2_32.lib")
 #include <winsock.h>
 #include <ws2tcpip.h>
#else
 #include <sys/types.h>
 #include <sys/socket.h> 
 #include <arpa/inet.h>
 #include <sys/ioctl.h>
 #include <net/if.h>
#endif
#ifdef __WIN32
 #define NET_ARCH SOCKET
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
 #define NET_ARCH int
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
// 1X - Any side, runtime error or warning (user's algorithm call some exceptions)          runtime
// 2X - Any side, settings or using error                                                   Human error
// 3X - Any side, Iternel errors (architecture fail)                                        arch error
// 4X - Any side, Called from outside                                                       OS-called
// 5X - Any side, Preparing stage fail                                                      human error, setup error
// 6X - Any side, Hardware or OS error                                                      OS error (or hard)
// 7X - Any side, while connection is active                                                runtime
// 8X - Server fail
// 9X - Client fail
// Additional codes:
                    //Connection refused: closed from other side.
                    #define SOCKEXC_DISCONN         0x70
                    //Connection refused: closed or broken.
                    #define SOCKEXC_BROKENP         0x71
                    //Connection refused: time out.
                    #define SOCKEXC_TIMEOUT         0x72
                    //Failed to bind server: already in use.
                    #define SOCKEXC_ALINUSE         0x80
                    //OS arch is unsupported.
                    #define SOCKEXC_ARCHUNS         0x50
                    //Wrong listener list: one or more listeners is conflicting.
                    #define SOCKEXC_UNCERTL         0x20
                    //NULL using as buffer
                    #define SOCKEXC_NULLBUF         0x21
                    //Iternal error excite exception. in additional info taken struct with all info.
                    #define SOCKEXC_ITERNAL         0x30
                    //OS unique error.
                    #define SOCKEXC_ARCHERR         0x31
                    //Connection denied: host not found or not resolved.
                    #define SOCKEXC_RESOLVE         0x91
                    //No file description for socket.
                    #define SOCKEXC_NOFILED         0x32
                    //WSA_INVALID_HANDLE.
                    #define SOCKEXC_INVHAND         0x33
                    //Not enough memory.
                    #define SOCKEXC_NOENMEM         0x60
                    //Invalid parameter from socket opt.
                    #define SOCKEXC_OSINPAR         0x34
                    //Operation aborted.
                    #define SOCKEXC_ABORTED         0x61
                    //Overlapped I/O event object not in signaled state.
                    #define SOCKEXC_IOINCOM         0x62
                    //Overlapped operations will complete later. 
                    #define SOCKEXC_WSAIOIN         0x63
                    //Interrupted function call from WSA manager.
                    #define SOCKEXC_WSAINTR         0x64
                    //WSA Permission denied.
                    #define SOCKEXC_WSAEACC         0x65
                    //File handle is not valid.
                    #define SOCKEXC_WSAEBAD         0x66
                    //Too many open sockets.
                    #define SOCKEXC_TOOMANY         0x67
                    //No buffer.
                    #define SOCKEXC_NOENBUF         0x32

#define endStream(x)        return(LAMBDA_THREAD_END(x))// type endStream(exit code); for break listener's loop
#define thread_routine_t    LAMBDA_THREAD_END           // type of poiner to function for thread
#define thread_t            THREADING_ARCH              // type of thread
#define socket_t            NET_ARCH                    // type of socket file description

class ConnectionList{
    protected:
        ConnectionList *next;
    public:
        ConnectionList(socket_t fd=-1, thread_t thread=0x0);

        socket_t conn;
        thread_t thread;

        ConnectionList& operator[](int index);

        int append(socket_t conn, thread_t thread);
        int insert(socket_t conn, thread_t thread);
        int remove();
        int length();

        int full();
};

class Socket{
    public:
        //NT-only declire
        #ifdef __WIN32
         WSADATA wsa;
         struct addrinfo *result = NULL, *ptr = NULL, hints;
        #endif
        /** Comment this later **/
        Socket();
        char *buffer;
        int bitrate;
        struct sockaddr_in address; 
        socket_t fd;
        int cc;
        ConnectionList connection;
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
        int setBitrate(int bitrate);
        char *setBuffer(char *buffer);
};

class ___SockPTR___uniQED__{
    public:
        ___SockPTR___uniQED__(void *client, bool isServer);
        socket_t *fd;
        ConnectionList *conn;
        int *cc;
        int *bitrate;
        char *buffer;
        /** Communicate with connection **/
        std::string read();
        std::string read(int conn);
        int send(std::string data);
        int send(std::string data, int conn);
        /** Connection managment; **/
        int kill(int conn);
        int close(int conn);
        int setBitrate(int bitrate);
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

        #ifdef __WIN32
        void buildFromWSA(int err);
        #endif
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
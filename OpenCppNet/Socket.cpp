#pragma once
#ifndef SCFD
#include "./Socket.hpp"
#endif

/** Basic socket interpritation **/
int SSLLBC(int __fd, const sockaddr *__addr, long long __len) throw(){
    return bind (__fd, __addr, __len);
}
ssize_t readLS(int __fd, void *__buf, size_t __nbytes){
    return read(__fd, __buf, __nbytes);
}
ssize_t sendLS(int __fd, const char *__buf, size_t __n, int __flags){
    return send(__fd, __buf, __n, __flags);
}
int connectLS(int __fd, const sockaddr *__addr, long long __len){
    return connect(__fd, __addr, __len);
}
int listenLS(int __fd, int __n){
    return listen(__fd, __n);
}
int sideTo(int *array, int start, int end){
    for(int i = start; i < end; i++){
        array[i] = array[i+1];
    }
    return 0;
}
int sideTo(int **array, int start, int end){
    for(int i = start; i < end -1; i++){
        array[i] = array[i+1];
    }
    return 0;
}

/** Basic socket interface **/
int Socket::send(std::string data){
    return sendLS(*connection[cc-1], data.c_str(), this->bitrade, 0);
}
int Socket::send(std::string data, int conn){
    return sendLS(*connection[conn], data.c_str(), this->bitrade, 0);
}
std::string Socket::read(){
    int sub = readLS(*connection[cc-1], buffer, this->bitrade);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    if(sub == 0){
        throw SocketException("localhost", "Connection closed from other side", (char*)"discon", 60012, 0x71);
        //connection closerd
    }else if(sub < 0){
        throw SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound", 60012, 0x72);
        //connection already destroyed
    }
    else return std::string(buffer); // connection active
}
std::string Socket::read(int conn){
    int sub = readLS(*connection[conn], buffer, this->bitrade);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    if(sub == 0){
        throw SocketException("localhost", "Connection closed from other side", (char*)"discon", 60012, 0x71);
        //connection closerd
    }else if(sub < 0){
        throw SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound", 60012, 0x72);
        //connection already destroyed
    }
    return std::string((char*)(buffer));
}
SocketException::SocketException(const char *address, const char *description, const char *additional, int port, int code, int additionalCode){
    // this->address = address; this->description = description; this->additional = additional; this-> port = port; this->codeOfError = code;
    this->address = (char*)malloc(strlen(address));
    strcpy(this->address, address);
    this->description = (char*)malloc(strlen(description));
    strcpy(this->description, description);
    this->additional = (char*)malloc(strlen(additional));
    strcpy(this->additional, additional);
    this->port = port;
    this->codeOfError = code;
    this->additionalCode = additionalCode;
}
void SocketException::print(){
    std::cout<<this->description<<"\n\tAddress:"<<this->address<<"\n\tPort:"<<this->port<<"\n\taAdditional info:"<<this->additional<<"\n\tCode of error:"<<\
    this->codeOfError << "\n\t\tSub-code: " << this->additionalCode << std::endl;
}
void SocketException::call(){
    #ifdef __NOGDB
    print();
    #endif
    throw *this;
}

Socket::Socket(){
    #ifdef __WIN32
    if(WSAStartup(MAKEWORD(2,2), &this->wsa) != 0)
        SocketException("Operating system arch", "Failed to setup WSA data", (char*)&this->wsa, SOCKEXC_ARCHERR, SOCKEXC_ARCHERR).call();
    #endif
    bitrade = 0;
}
char *Socket::setBuffer(char *buffer){
    this->buffer = buffer;
    return buffer;
}
int Socket::setBitrade(int bitrade){
    if(bitrade != 0)
        this->bitrade = bitrade;
    return this->bitrade;
}
int Socket::kill(int conn){
    this->connection.erase(connection.begin()+conn);
    return 0;
}
// int Socket::stop(int conn){

// }

/** socket Server interface **/

SocketServer::SocketServer(){
    Socket();
    #ifndef __WIN32
     this->threadStorage = new pthread_t*;
    pthread_mutexattr_init(&this->mutexattr);
    pthread_mutex_init(&this->mutex, &mutexattr);
    #endif
}
int SocketServer::bind(std::string addr, int port, int protocol, long additionalFlags){
    this->buffer = new char[this->bitrade];
    this->me = addr;    this->port = port;
    fd = socket(AF_INET, protocol, IPPROTO_TCP);
    #ifndef __WIN32
     setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    #else
     setsockopt(fd, SOL_SOCKET, additionalFlags, &opt, sizeof(opt));
    #endif
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 
    // this->arg.sockPtr = new ___SockPTR___uniQED__(this, 1);
    char exitCode = 0;
    #ifdef __WIN32
    if((exitCode = SSLLBC(fd, (struct sockaddr *)&address, sizeof(address)))==SOCKET_ERROR){
        exitCode = WSAGetLastError();
        SocketException((char*)addr.c_str(), (char*)"Failed to bind server: Host not resolved\
 or already in use.", (char*)exitCode, port, -2).call();
    }
    #else
    if((exitCode = SSLLBC(fd, (struct sockaddr *)&address, sizeof(address))))
        SocketException((char*)addr.c_str(), (char*)"Failed to bind server: Host not resolved\
 or already in use.", &exitCode, port, -2).call();
    #endif
    #ifndef __WIN32
    if (fd == 0)
    #else
    if (fd == INVALID_SOCKET)
        SocketException((char*)addr.c_str(), (char*)"Failed to bind server: file description\
is null.", "iternal error", port, -1).call();
    #endif
    if(opt < 0)
        SocketException((char*)addr.c_str(), (char*)"Failed to bind server: socket options\
is null or not resolved.", "sockopt is invalid", port, -3).call();

    return(address.sin_port);
}
thread_routine_t SocketServer::listen(void *arg){
    int errcode;
    throwenSocketServerStruct *me = (throwenSocketServerStruct*)(arg);
    #ifndef __WIN32
     pthread_mutex_lock(&me->socketServer->mutex);
    #endif
    while(1){
        printf("loop step ready\n");
        // if(me->socketServer->cc != -1)
        //     pthread_destroy()
        // int y = 0;
        // for(;me->sockPtr->conn[y] != NULL; y++)
        //     printf("%i connection is busy\n", y);
        // printf("Wait for connection on [%i] slot\n", y);
        me->socketServer->connection.push_back(0);
        me->socketServer->threadStorage.push_back((thread_t*)malloc(sizeof(thread_t)));
        int y = me->socketServer->connection.size();
        #ifndef __WIN32
        if (errcode = listenLS(me->socketServer->fd, 3) < 0)
        #else
        if (errcode = listenLS(me->socketServer->fd, 3) == SOCKET_ERROR)
        #endif
            SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", (char*)"(ListenLS)See additionalErrorCode for more info",\
            me->socketServer->port, SOCKEXC_ITERNAL, errcode).call();
        sleep(1);
        #ifndef __WIN32
        if ((me->socketServer->connection[y] = new int(accept(me->socketServer->fd, (struct sockaddr *)&me->socketServer->address,  
                        (int*)&me->socketServer->al)))<0)
        #else
        int sub = accept(me->socketServer->fd, (struct sockaddr *)&me->socketServer->address, (int*)&me->socketServer->al);
        if ((me->socketServer->connection[y] = new int(accept(me->socketServer->fd, (struct sockaddr *)&me->socketServer->address,  
                        (int*)&me->socketServer->al)))!=NULL && sub == INVALID_SOCKET)
        #endif
            SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", (char*)"accept fail", me->socketServer->port, SOCKEXC_ITERNAL, errcode).call();
        #ifdef SOCKET_STDOUT_REPORTING
            SOCKET_REPORTING("New user connected\n");
        #endif
        me->connectionID = y;
        me->socketServer->threadStorage[y] = (thread_t*) malloc(sizeof(thread_t));
        if(me->socketServer->listener == nullptr){
            #ifdef USE_PTHREAD
             pthread_create(me->socketServer->threadStorage[y], &me->socketServer->attr, me->listener, new throwNetAttr(me, me->connectionID));
             pthread_detach(*me->socketServer->threadStorage[y]);
            #else
             *me->socketServer->threadStorage[y] = CreateThread(NULL, 0, me->listener, new throwNetAttr(me, me->connectionID), 0, NULL);
            #endif
            // printf("Func");
        }else{
            // if(me->socketServer->listener->)
            #ifdef USE_PTHREAD
            pthread_create(me->socketServer->threadStorage[y], &me->socketServer->attr, me->socketServer->listener->Upthreadable, new throwNetAttr(me, me->connectionID, me->socketServer->listener));
            pthread_detach(*me->socketServer->threadStorage[y]);
            #else
             *me->socketServer->threadStorage[y] = CreateThread(NULL, 0, me->listener, new throwNetAttr(me, me->connectionID), 0, NULL);
            #endif
            y++;
            me->socketServer->connection[y] = new int(0);
            me->socketServer->threadStorage[y] = (thread_t*)malloc(sizeof(thread_t));
            #ifdef USE_PTHREAD
             pthread_create(me->socketServer->threadStorage[y], &me->socketServer->attr, me->socketServer->listener->Downthreadable, new throwNetAttr(me, me->connectionID, me->socketServer->listener));
             pthread_detach(*me->socketServer->threadStorage[y]);
            #else
             *me->socketServer->threadStorage[y] = CreateThread(NULL, 0, me->listener, new throwNetAttr(me, me->connectionID), 0, NULL);
            #endif
        }
        me->socketServer->cc++;
    }
    #ifdef USE_PTHREAD
     pthread_mutex_unlock(&me->socketServer->mutex);
    #endif
}
void SocketServer::start(thread_routine_t (*listener)(void *arg), int maxClients){
    this->arg = {this, listener};
	this->arg.sockPtr = new ___SockPTR___uniQED__(___SockPTR___uniQED__(this, 1));
    #ifdef __USE_PTHREAD
    pthread_create(&mainThread, &attr, this->listen, &arg);
    pthread_detach(mainThread);
    #else
    this->mainThread = CreateThread(NULL, 0, this->listen, &arg, NULL, NULL);
    #endif
}
void NetworkListener::Set___SockPTR___uniQED__(___SockPTR___uniQED__ *sockptr){
    this->listeners.arg = sockptr;
}
void SocketServer::start(NetworkListener *listener, int maxClients){
    this->arg = {this, nullptr};
    this->listener = listener;
	this->arg.sockPtr = new ___SockPTR___uniQED__(___SockPTR___uniQED__(this, 1));
    #ifdef USE_PTHREAD
     pthread_create(&mainThread, &attr, this->listen, &arg);
     pthread_detach(mainThread);
    #else
     this->mainThread = CreateThread(NULL, 0, this->listen, &arg, NULL, NULL);
    #endif
}

/** socket Client interface **/
SocketClient::SocketClient(){
    Socket();
}
int SocketClient::connect(std::string addr, int port, int protocol){
    fd = socket(AF_INET, protocol, 0);
    if(fd == INVALID_SOCKET) SocketException(addr.c_str(), "Failed to get socket file description", (char*)&fd, port, SOCKEXC_NOFILED, 0x90).call();
    address.sin_family = AF_INET; 
    address.sin_port = htons(port); 
    #ifdef __WIN32
     address.sin_addr.s_addr = inet_addr(addr.c_str());
    #else
     if(inet_pton(AF_INET, addr.c_str(), &address.sin_addr)<=0)
         SocketException((char*)addr.c_str(), "Failed to connect to server: Host not resolved\
 or already in use. Failed on settings stage.", nullptr, port, -1).call();
    #endif
    if (connectLS(fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
        SocketException((char*)addr.c_str(), "Failed to connect to server: Host not resolved\
 or already in use. Failing in connection stage.", nullptr, port, -2).call();
    connection[0] = &fd;
    cc++;
    return(address.sin_port);
}

/*** NetworkListener interface **/


void NetworkListener::setListener(int type, void (*listener)(void *attr)){
    if(type == DEFAULT_LISTENER)
        this->listeners.defaultListener = listener;
    else if(type == UPLOADS_LISTENER)
        this->listeners.upstream = listener;
    else if(type == DOWNLOAD_LISTENER)
        this->listeners.downstream = listener;
    else SocketException((char*)"localhost/programm", (char*)"unknowen listener type", (char*)std::to_string(type).c_str(), 0x000, -9).call();
}

std::string ___SockPTR___uniQED__::read(){
    char *tmp = new char[*this->bitrade];
    readLS(*conn[*cc-1], tmp, *this->bitrade);
    if(tmp == NULL) SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's (or buffer) is NULL", (char*)"called from ___SockPTR::read() May be all sock-ptr struct was delted.", 71, -1).call();
    free(tmp);
    std::string str = tmp;
    return str;
}
std::string ___SockPTR___uniQED__::read(int conn){
    char *tmp = new char[*this->bitrade];
    int sub = readLS(*this->conn[conn], tmp, *this->bitrade);
    if(sub == 0){
        SocketException("localhost", "Connection closed from other side", (char*)"discon, reading trial.", 60012, 0x71).call();
        //connection closerd
    }else if(sub < 0){
        SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound, reading trial.", 60012, 0x72).call();
        //connection already destroyed
    }
    if(tmp == nullptr) SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's (or buffer) is NULL", (char*)"called from ___SockPTR::read(int conn). May be all sock-ptr struct was delted.", 71, -1).call();
    std::string str = tmp;
    free(tmp);
    return str;
}
int ___SockPTR___uniQED__::send(std::string data){
    return sendLS(*conn[*cc-1], data.c_str(), *this->bitrade, 0);
}
int ___SockPTR___uniQED__::send(std::string data, int conn){
    return sendLS(*this->conn[conn], data.c_str(), *this->bitrade, 0);
}
char *___SockPTR___uniQED__::setBuffer(char *buffer){
    this->buffer = buffer;
    return buffer;
}
int ___SockPTR___uniQED__::setBitrade(int bitrade){
    *this->bitrade = bitrade;
    return bitrade;
}
___SockPTR___uniQED__::___SockPTR___uniQED__(void *ptr, bool isServer){
    if(isServer){
        SocketServer *server = (SocketServer*)(ptr);
        this->fd = &server->fd;
        this->conn = server->connection;
        this->buffer = server->buffer;
        this->bitrade = &server->bitrade;
        this->cc = &server->cc;
    }else{
        SocketClient *client = (SocketClient*)(ptr);
        this->fd = &client->fd;
        this->conn = client->connection;
        this->buffer = client->buffer;
        this->bitrade = &client->bitrade;
        this->cc = &client->cc;
    }
}

___SockPTR___uniQED__* getSockPointerFromAttr(SocketServer::throwenSocketServerStruct *attr){
	return attr->sockPtr;
}

throwNetAttr::throwNetAttr(netServerAttribures *attr, int conn){
    this->ptsd = attr;
    this->ptsd->sockPtr->connn = this->connID = conn;
    this->listener = nullptr;
}
throwNetAttr::throwNetAttr(netServerAttribures *attr, int conn, NetworkListener *listener){
    this->ptsd = attr;
    this->ptsd->sockPtr->connn = this->connID = conn;
    this->listener = listener;
}

void *NetworkListener::threadable(void *arg){
    throwNetAttr *me = (throwNetAttr*)(arg);
    if(me->listener->listeners.defaultListener != NULL &&\
        me->listener->listeners.upstream != NULL &&\
        me->listener->listeners.downstream != NULL)
            SocketException("localhost:server", "uncertain type of listeners", "You should take only DEFAULT_LISTENER or UPLOADS_LISTENER and DOWNLOAD_LISTENER", 0, SOCKEXC_UNCERTL).call();
    else if(me->listener->listeners.defaultListener != NULL)
        me->listener->listeners.defaultListener(arg);
    else SocketException("localhost:server", "uncertain type of listeners", "You should take only DEFAULT_LISTENER or UPLOADS_LISTENER and DOWNLOAD_LISTENER", 241, SOCKEXC_UNCERTL).call();
    return 0;
}

void *NetworkListener::Upthreadable(void *arg){
    throwNetAttr *me = (throwNetAttr*)(arg);
    if(me->listener->listeners.defaultListener != NULL &&\
        me->listener->listeners.upstream != NULL &&\
        me->listener->listeners.downstream != NULL)
            SocketException("localhost:server", "uncertain type of listeners", "You should take only DEFAULT_LISTENER or UPLOADS_LISTENER and DOWNLOAD_LISTENER", 0, SOCKEXC_UNCERTL).call();
    else if(me->listener->listeners.upstream != NULL){
        me->listener->listeners.upstream(arg);
    }
    else SocketException("localhost:server", "uncertain type of listeners", "You should take only DEFAULT_LISTENER or UPLOADS_LISTENER and DOWNLOAD_LISTENER", 241, SOCKEXC_UNCERTL).call();
    return 0;
}

void *NetworkListener::Downthreadable(void *arg){
    throwNetAttr *me = (throwNetAttr*)(arg);
    if(me->listener->listeners.defaultListener != NULL &&\
        me->listener->listeners.upstream != NULL &&\
        me->listener->listeners.downstream != NULL)
            SocketException("localhost:server", "uncertain type of listeners", "You should take only DEFAULT_LISTENER or UPLOADS_LISTENER and DOWNLOAD_LISTENER", 0, SOCKEXC_UNCERTL).call();
    else if(me->listener->listeners.downstream != NULL){
        me->listener->listeners.downstream(arg);
    }
    else SocketException("localhost:server", "uncertain type of listeners", "You should take only DEFAULT_LISTENER or UPLOADS_LISTENER and DOWNLOAD_LISTENER", 241, SOCKEXC_UNCERTL).call();
    return 0;
}

___SockPTR___uniQED__ *getAttributes(void *pointer){
    return (___SockPTR___uniQED__*)(pointer);
}

template <typename mutualType>
void ListenerStream<mutualType>::setBuffer(char *buffer){
    this->attributes->ptsd->sockPtr->setBuffer(buffer);
    this->buffer = buffer;
}
template <typename mutualType>
std::string ListenerStream<mutualType>::read(){
    return this->attributes->ptsd->sockPtr->read(this->attributes->connID);
}
template <typename mutualType>
void ListenerStream<mutualType>::send(std::string data){
    int sub = this->attributes->ptsd->sockPtr->send(data, this->attributes->connID);
    if(sub == 0){
        SocketException("localhost", "Connection closed from other side", (char*)"discon, sending trial", 60012, 0x71).call();
        //connection closerd
    }else if(sub < 0){
        SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound, sending trial", 60012, 0x72).call();
        //connection already destroyed
    }
}
template <typename mutualType>
ListenerStream<mutualType>::ListenerStream(void *arg){
    this->attributes = (throwNetAttr*)(arg);
    std::cout << this->attributes->ptsd->sockPtr->fd << std::endl;
    //this->attributes->mutualRes = malloc(sizeof(mutualRed));
    if(this->attributes->mutualRes == NULL)
        this->attributes->mutualRes = malloc(sizeof(mutualType));
}
template <typename mutualType>
void ListenerStream<mutualType>::close(){
    (*(this->attributes->ptsd->sockPtr->cc))--;
    // sideTo(this->attributes->ptsd->sockPtr->conn, this->attributes->connID, *(this->attributes->ptsd->sockPtr->cc)+2);
    delete(this->attributes->ptsd->sockPtr->conn[this->getCurrent()]);
    delete(this->attributes->ptsd->socketServer->threadStorage[this->getCurrent()]);
    this->attributes->ptsd->socketServer->threadStorage[this->getCurrent()] = NULL;
    this->attributes->ptsd->sockPtr->conn[this->getCurrent()] = NULL;
}
template <typename mutualType>
int ListenerStream<mutualType>::getCurrent(){
    return this->attributes->connID;
}
int SocketClient::read(){
    int sub = readLS(*connection[cc-1], buffer, this->bitrade);
    if(buffer == NULL) SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1).call();
    if(sub == 0){
        SocketException("localhost", "Connection closed from other side", (char*)"discon", 60012, 0x71).call();
        //connection closerd
    }else if(sub < 0){
        SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound", 60012, 0x72).call();
        //connection already destroyed
    }
    return sub;
}
bool NetworkListener::isMonolite(){
    return this->listeners.defaultListener != NULL;
}
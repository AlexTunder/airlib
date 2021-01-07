#pragma once
#ifndef SCFD
#include "./Socket.hpp"
#endif

#ifdef __WIN32
int ExceptionCodeFromWSA(int wsa){
    switch(wsa){
        case WSA_INVALID_HANDLE:
            return SOCKEXC_NOENMEM;
        case WSA_NOT_ENOUGH_MEMORY:
            return SOCKEXC_NOENMEM;
        case WSA_INVALID_PARAMETER:
            return SOCKEXC_OSINPAR;
        case WSA_OPERATION_ABORTED:
            return SOCKEXC_WSAEACC;
        case WSA_IO_INCOMPLETE:
            return SOCKEXC_IOINCOM;
        case WSA_IO_PENDING:
            return SOCKEXC_WSAIOIN;
        case WSAEINTR:
            return SOCKEXC_WSAINTR;
        case WSAEBADF:
            return SOCKEXC_WSAEBAD;
        case WSAEACCES:
            return SOCKEXC_WSAEACC;
    }
    return SOCKEXC_ARCHERR;
}
char *WSAToStr(int excCode){
    char *sub;
    if(excCode == SOCKEXC_NOENMEM){
        sub = (char*)malloc(strlen("Not enoght memory"));
        strcpy(sub, "Not enoght memory");
    } else
    if(excCode == SOCKEXC_OSINPAR){
        sub = (char*)malloc(strlen("Invalid parameter from socket opt."));
        strcpy(sub, "Invalid parameter from socket opt.");
    } else
    if(excCode == SOCKEXC_WSAEACC){
        sub = (char*)malloc(strlen("WSA Permission denied."));
        strcpy(sub, "WSA Permission denied.");
    } else
    if(excCode == SOCKEXC_IOINCOM){
        sub = (char*)malloc(strlen("WSA Permission denied."));
        strcpy(sub, "WSA Permission denied.");
    } else{
        sub = (char*)malloc(1024);
        sprintf(sub, "Unknowen Error code: %i", excCode);
    }
    return sub;
}
#endif

/** Basic socket interpritation **/
int SSLLBC(int __fd, const sockaddr *__addr, long long __len) throw(){
    return bind (__fd, __addr, __len);
}
ssize_t readLS(int __fd, void *__buf, size_t __nbytes){
    #ifndef __WIN32
    return read(__fd, __buf, __nbytes);
    #else
    recv(__fd, (char*)__buf, __nbytes, 0);
    #endif
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
    return sendLS(connection[cc-1].conn, data.c_str(), this->bitrate, 0);
}
int Socket::send(std::string data, int conn){
    return sendLS(connection[conn].conn, data.c_str(), this->bitrate, 0);
}
std::string Socket::read(){
    int sub = readLS(connection[cc-1].conn, buffer, this->bitrate);
    if(buffer == NULL) throw(SocketException((char*)"localhost", (char*)"Failed to read incoming data, \
becouse buffer is NULL", nullptr, SOCKEXC_NULLBUF, -1));
    #ifndef __WIN32
    if(sub == 0){
        SocketException("localhost", "Connection closed from other side", (char*)"discon", 60012, 0x71).call();
        //connection closerd
    }else if(sub < 0){
        SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound", 60012, 0x72).call();
        //connection already destroyed
    }
    #else
    if(sub == SOCKET_ERROR){
        int lastCode = WSAGetLastError();
        SocketException("localhost", WSAToStr(ExceptionCodeFromWSA(lastCode)), "called from read func.", 60012, ExceptionCodeFromWSA(lastCode)).call();
    }else printf("Sub is not -1, cuz it's a %i\n", sub);
    #endif
    return std::string(buffer); // connection active
}
std::string Socket::read(int conn){
    int sub = readLS(connection[conn].conn, buffer, this->bitrate);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    #ifndef __WIN32
    if(sub == 0){
        throw SocketException("localhost", "Connection closed from other side", (char*)"discon", 60012, 0x71);
        //connection closerd
    }else if(sub < 0){
        throw SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound", 60012, 0x72);
        //connection already destroyed
    }
    #else
    if(sub == SOCKET_ERROR){
        int lastCode = WSAGetLastError();
        SocketException("localhost", WSAToStr(ExceptionCodeFromWSA(lastCode)), "called from read func.", 60012, ExceptionCodeFromWSA(lastCode)).call();
    }
    #endif
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
    this->fd = INVALID_SOCKET;
    if(WSAStartup(MAKEWORD(2,2), &this->wsa) != 0)
        SocketException("Operating system arch", "Failed to setup WSA data", (char*)&this->wsa, SOCKEXC_ARCHERR, SOCKEXC_ARCHERR).call();
    #endif
    bitrate = 0;
}
/*
* Set reading buffer for socket
*
*@param buffer pointer to (char*)buffer
*@return current buffer
*
*/
char *Socket::setBuffer(char *buffer){
    this->buffer = buffer;
    return buffer;
}
/*
* Set reading bitrate for socket
*
*@param bitrate new bitrate from current socket
*@return current bitrate or -1 as error
*
*/
int Socket::setBitrate(int bitrate){
    if(bitrate != 0)
        this->bitrate = bitrate;
    return this->bitrate;
}
/*
* Close connection and clean up all data
*
*@param conn index of connection
*@return 0 or -1 as error
*
*/
int Socket::kill(int conn){
    closesocket(this->connection[conn].conn);
    this->connection[conn].remove();
    return 0;
}

/** socket Server interface **/

SocketServer::SocketServer(){
    Socket();
    #ifndef __WIN32
     this->threadStorage = new pthread_t*;
    pthread_mutexattr_init(&this->mutexattr);
    pthread_mutex_init(&this->mutex, &mutexattr);
    #endif
}
/*
* Bind server and make setup
*
*@param addr Address of server. Usually it's localhost (if this is testing) or domain name of current machine
*@param port Integer port for binding
*@param protocol Using TCP or UDP protocol. By default - TCP
*@param additionalFlag Using additional settings for binding in configuring socket options
*@return Real numeric port
*
*@return 0 or -1 as error
*
*/
int SocketServer::bind(std::string addr, int port, int protocol, long additionalFlags){
    this->buffer = new char[this->bitrate];
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
 or already in use.", (char*)char(exitCode), port, -2).call();
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
    #endif
        SocketException((char*)addr.c_str(), (char*)"Failed to bind server: file description\
is null.", "iternal error", port, -1).call();
    if(opt < 0)
        SocketException((char*)addr.c_str(), (char*)"Failed to bind server: socket options\
is null or not resolved.", "sockopt is invalid", port, -3).call();

    return(address.sin_port);
}
/*
*
* Iternal function for start client listening
*
* @param arg Pointer to object of throwenSocketServerStruct
* @return 0x0
*
*/
thread_routine_t SocketServer::listen(void *arg){
    int errcode;
    throwenSocketServerStruct *me = (throwenSocketServerStruct*)(arg);
    #ifndef __WIN32
     pthread_mutex_lock(&me->socketServer->mutex);
    #endif
    while(1){
        printf("loop step ready\n");

        int y = 0;

        if(!me->socketServer->connection.full()) 
            y = me->socketServer->connection.insert(0, 0);
        else y = me->socketServer->connection.append(0, 0);

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
        if ((me->socketServer->connection[y].conn = accept(me->socketServer->fd, (struct sockaddr *)&me->socketServer->address,  
                        (int*)&me->socketServer->al)) != 0 && sub == INVALID_SOCKET)
        #endif
            SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", (char*)"accept fail", me->socketServer->port, SOCKEXC_ITERNAL, errcode).call();
        #ifdef SOCKET_STDOUT_REPORTING
            SOCKET_REPORTING("New user connected\n");
        #endif
        me->connectionID = y;
        if(me->socketServer->listener == nullptr){
            throwNetAttr *thr = (throwNetAttr*)malloc(sizeof(throwNetAttr));
            *thr = throwNetAttr(me, me->connectionID);
            #ifdef USE_PTHREAD
             pthread_create(me->socketServer->threadStorage[y], &me->socketServer->attr, me->listener, new throwNetAttr(me, me->connectionID));
             pthread_detach(*me->socketServer->threadStorage[y]);
            #else
             me->socketServer->connection[y].thread = CreateThread(NULL, 0, me->listener, thr, 0, NULL);
            #endif
            // printf("Func");
        }else{
            throwNetAttr *thr = (throwNetAttr*)malloc(sizeof(throwNetAttr));
            *thr = throwNetAttr(me, me->connectionID);
            #ifdef USE_PTHREAD
            pthread_create(me->socketServer->threadStorage[y], &me->socketServer->attr, me->socketServer->listener->Upthreadable, new throwNetAttr(me, me->connectionID, me->socketServer->listener));
            pthread_detach(*me->socketServer->threadStorage[y]);
            #else
             me->socketServer->connection[y].thread = CreateThread(NULL, 0, me->listener, thr, 0, NULL);
            #endif
            y++;
            if(me->socketServer->connection.full() < 100) 
                y = me->socketServer->connection.insert(0, 0);
            else y = me->socketServer->connection.append(0, 0);
            #ifdef USE_PTHREAD
             pthread_create(me->socketServer->threadStorage[y], &me->socketServer->attr, me->socketServer->listener->Downthreadable, new throwNetAttr(me, me->connectionID, me->socketServer->listener));
             pthread_detach(*me->socketServer->threadStorage[y]);
            #else
             me->socketServer->connection[y].thread = CreateThread(NULL, 0, me->listener, new throwNetAttr(me, me->connectionID), 0, NULL);
            #endif
        }
        me->socketServer->cc++;
    }
    #ifdef USE_PTHREAD
     pthread_mutex_unlock(&me->socketServer->mutex);
    #endif
    return 0x0;
}
/*
*
* Start server daemon
*
*@param listener pointer to function with listener
*@param maxClients count of max aviable connections
*@return nothing
*
*/
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
/*
*
* Setup usiversal pointer
*
*@param sockptr pointer to pointer (:D)
*@return nothing
*
*/
void NetworkListener::Set___SockPTR___uniQED__(___SockPTR___uniQED__ *sockptr){
    this->listeners.arg = sockptr;
}
/*
*
* Start server daemon
*
* Best variant for server daemon
*
*@param listener pointer to NetworkListener class with listener(s)
*@param maxClients count of max aviable connections
*@return nothing
*
*/
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
/*
*
* Connect client to server
*
*@param addr Address (ip or domain name) for pairing
*@param port Integer port for connection (aka IP:PORT)
*@param protocol TCP or other protocol
*@return Real int port
*
*/
int SocketClient::connect(std::string addr, int port, int protocol){
     printf("[info] protocol: %i\n",protocol);
    fd = socket(AF_INET, protocol, IPPROTO_TCP);
    if(fd == INVALID_SOCKET)
        throw SocketException(addr.c_str(), "Failed to get socket file description", (char*)&fd, port, SOCKEXC_NOFILED, fd);
    printf("fd: %i\nwsa checkout:%i\t", fd, WSAGetLastError());
    memset(&address, 0, sizeof(addr));
    address.sin_family = AF_INET;
    #ifdef __WIN32
     printf("ADDR: \'%s\'\n", addr.c_str());
     if((address.sin_addr.s_addr = inet_addr(addr.c_str())) == INADDR_NONE )
        {printf("INADDR_NONE fail\n"); throw -1;}
    else printf("addr setup ok (%i)\n", address.sin_addr.s_addr);
    #else
     if(inet_pton(AF_INET, addr.c_str(), &address.sin_addr)<=0)
         SocketException((char*)addr.c_str(), "Failed to connect to server: Host not resolved\
 or already in use. Failed on settings stage.", nullptr, port, -1).call();
    #endif
    address.sin_port = htons(port);
    printf("If I'll crash right here, it's mean, what error with address.\n");
    int sub = 0;
    sub = connectLS(fd, (struct sockaddr*)&address, sizeof(address));
    if (sub == SOCKET_ERROR)
        {printf("Failed Failed to connect to server: Host not resolved\
 or already in use. Failing in connection stage (%i, wsa: %i). Struct pointer: %p/%p\n", sub, WSAGetLastError(), (struct sockaddr*)&address, &address); throw -1; }
//         throw SocketException((char*)addr.c_str(), "Failed to connect to server: Host not resolved\
// //  or already in use. Failing in connection stage.", nullptr, port, -2);
    // connection.append(fd,)
    printf("conn len: %i\n", connection.length());
    // connection[0].conn = fd;
    cc++;
    return(address.sin_port);
}

/*** NetworkListener interface **/

/*
*
* Connect client to server
*
*@param type Listener type: download, upload or echo listener indentifier
*@param listener Use [](void*){...} to define listener
*@return nothing
*
*/
void NetworkListener::setListener(int type, void (*listener)(void *attr)){
    if(type == DEFAULT_LISTENER)
        this->listeners.defaultListener = listener;
    else if(type == UPLOADS_LISTENER)
        this->listeners.upstream = listener;
    else if(type == DOWNLOAD_LISTENER)
        this->listeners.downstream = listener;
    else SocketException((char*)"localhost/programm", (char*)"unknowen listener type", (char*)std::to_string(type).c_str(), 0x000, -9).call();
}

/*
*
* Read incoming data
*
*@return string with data
*
*/
std::string ___SockPTR___uniQED__::read(){
    char *tmp = new char[*this->bitrate];
    readLS(conn[*cc-1].conn, tmp, *this->bitrate);
    if(tmp == NULL) SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's (or buffer) is NULL", (char*)"called from ___SockPTR::read() May be all sock-ptr struct was delted.", 71, -1).call();
    free(tmp);
    std::string str = tmp;
    return str;
}
/*
*
* Read incoming data
*
*@param conn Index of target for reading
*@return string with data
*
*/
std::string ___SockPTR___uniQED__::read(int conn){
    char *tmp = new char[*this->bitrate];
    int sub = readLS(this->conn[conn].conn, tmp, *this->bitrate);
    #ifndef __WIN32
    if(sub == 0){
        SocketException("localhost", "Connection closed from other side", (char*)"discon, reading trial.", 60012, 0x71).call();
        //connection closerd
    }else if(sub < 0){
        SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound, reading trial.", 60012, 0x72).call();
        //connection already destroyed
    }
    #else
        if(sub == SOCKET_ERROR){
            //here bug!
            int lastCode = WSAGetLastError();
            printf("sub != SOCKET_ERROR\n");
            SocketException("localhost", WSAToStr(ExceptionCodeFromWSA(lastCode)), "called from read func.", 60012, ExceptionCodeFromWSA(lastCode)).call();
        }
    #endif
    if(tmp == nullptr) SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's (or buffer) is NULL", (char*)"called from ___SockPTR::read(int conn). May be all sock-ptr struct was delted.", 71, -1).call();
    std::string str = tmp;
    free(tmp);
    return str;
}
int ___SockPTR___uniQED__::send(std::string data){
    return sendLS(conn[*cc-1].conn, data.c_str(), *this->bitrate, 0);
}
int ___SockPTR___uniQED__::send(std::string data, int conn){
    return sendLS(this->conn[conn].conn, data.c_str(), *this->bitrate, 0);
}
char *___SockPTR___uniQED__::setBuffer(char *buffer){
    this->buffer = buffer;
    return buffer;
}
int ___SockPTR___uniQED__::setBitrate(int bitrate){
    *this->bitrate = bitrate;
    return bitrate;
}
___SockPTR___uniQED__::___SockPTR___uniQED__(void *ptr, bool isServer){
    if(isServer){
        SocketServer *server = (SocketServer*)(ptr);
        this->fd = &server->fd;
        // this->conn = server->connection;
        this->buffer = server->buffer;
        this->bitrate = &server->bitrate;
        this->cc = &server->cc;
    }else{
        SocketClient *client = (SocketClient*)(ptr);
        this->fd = &client->fd;
        this->conn = &client->connection;
        this->buffer = client->buffer;
        this->bitrate = &client->bitrate;
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
    #ifndef __WIN32
    if(sub == 0){
        SocketException("localhost", "Connection closed from other side", (char*)"discon, sending trial", 60012, SOCKEXC_DISCONN).call();
        //connection closerd
    }else if(sub < 0){
        SocketException("localhost", "Connection is exsistn't or busy", (char*)"notfound, sending trial", 60012, SOCKEXC_BROKENP).call();
        //connection already destroyed
    }
    #else
    if(sub == SOCKET_ERROR){
        int lastCode = WSAGetLastError();
        SocketException("localhost", WSAToStr(ExceptionCodeFromWSA(lastCode)), "called from read func.", 60012, ExceptionCodeFromWSA(lastCode)).call();
    }
    #endif
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
    // delete(this->attributes->ptsd->sockPtr->conn[this->getCurrent()]);
    // delete(this->attributes->ptsd->socketServer->threadStorage[this->getCurrent()]);
    // this->attributes->ptsd->socketServer->connection.thread[this->getCurrent()] = 0x0;
    closesocket(this->attributes->ptsd->sockPtr->conn[this->getCurrent()].conn);
    this->attributes->ptsd->sockPtr->conn[this->getCurrent()].remove();
}
template <typename mutualType>
int ListenerStream<mutualType>::getCurrent(){
    return this->attributes->connID;
}
bool NetworkListener::isMonolite(){
    return this->listeners.defaultListener != NULL;
}
int ConnectionList::full(){
    int  y = 0;
    for(int i = 0; i <= this->length(); i++)
        if(this->operator[](i).conn == -1) y++;
    return y;
}
int ConnectionList::length(){
    if(next == NULL) return 0;
    else return next->length()+1;
}
ConnectionList& ConnectionList::operator[](int index){
    if(index == 0) return *this;
    else return this->next->operator[](index-1);
}
int ConnectionList::append(socket_t fd, thread_t thread){
    if(next == NULL){
        next = new ConnectionList(fd, thread);
        return 0;
    }else return next->append(fd, thread)+1;
}
ConnectionList::ConnectionList(socket_t fd, thread_t thread){
    this->conn = fd;
    this->thread = thread;
    this->next = NULL;
}
int ConnectionList::remove(){
    this->conn = -1;
    this->thread = 0x0;
    return -1;
}
int ConnectionList::insert(socket_t fd, thread_t thread){
    if(!this->full()) return this->append(fd, thread);
    else{
        int i = 0;
        for(; this->operator[](i).conn != -1; i++);
        this->operator[](i).conn = fd;
        this->operator[](i).thread = thread;
        return i;
    }
}
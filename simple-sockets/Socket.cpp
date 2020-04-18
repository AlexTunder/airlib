#include "./Socket.hpp"

/** Basic socket interpritation **/
int SSLLBC(int __fd, const sockaddr *__addr, socklen_t __len) throw(){
    return bind (__fd, __addr, __len);
}
ssize_t readLS(int __fd, void *__buf, size_t __nbytes){
    return read(__fd, __buf, __nbytes);
}
ssize_t sendLS(int __fd, const void *__buf, size_t __n, int __flags){
    return send(__fd, __buf, __n, __flags);
}
int connectLS(int __fd, const sockaddr *__addr, socklen_t __len){
    return connect(__fd, __addr, __len);
}
int listenLS(int __fd, int __n){
    return listen(__fd, __n);
}

/** Basic socket interface **/
int Socket::send(std::string data){
    return sendLS(*connection[cc-1], data.c_str(), this->bitrade, 0);
}
int Socket::send(std::string data, int conn){
    return sendLS(*connection[conn], data.c_str(), this->bitrade, 0);
}
std::string Socket::read(){
    readLS(*connection[cc-1], buffer, this->bitrade);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    return std::string((char*)(buffer));
}
std::string Socket::read(int conn){
    readLS(*connection[conn], buffer, this->bitrade);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    return std::string((char*)(buffer));
}
SocketException::SocketException(char *address, char *description, char *additional, int port, int code){
    this->address = address; this->description = description; this->additional = additional; this-> port = port; this->codeOfError = code;
}
Socket::Socket(){
    connection = new int*;
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

/** socket Server interface **/

SocketServer::SocketServer(){
    Socket();
    this->threadStorage = new pthread_t*;
}
int SocketServer::bind(std::string addr, int port, int protocol){
    this->buffer = new char;
    this->me = addr;    this->port = port;
    fd = socket(AF_INET, protocol, 0);
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(port); 
    if(SSLLBC(fd, (struct sockaddr *)&address, sizeof(address)))
        throw(SocketException((char*)addr.c_str(), (char*)"Failed to bind server: Host not resolved\
 or already in use.", nullptr, port, -2));
    if (fd == 0)
        throw(SocketException((char*)addr.c_str(), (char*)"Failed to bind server: file description\
is null.", nullptr, port, -1));
    if(opt < 0)
        throw(SocketException((char*)addr.c_str(), (char*)"Failed to bind server: socket options\
is null or not resolved.", nullptr, port, -3));
}
void *SocketServer::listen(void *arg){
    throwenSocketServerStruct *me = (throwenSocketServerStruct*)(arg);
    while(me->socketServer->state){
        me->socketServer->connection[me->socketServer->cc] = new int(0);
        me->socketServer->threadStorage[me->socketServer->cc] = new pthread_t();
        if (listenLS(me->socketServer->fd, 3) < 0)
            throw SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", "Start failed", me->socketServer->port, 0x61);
        if ((*me->socketServer->connection[me->socketServer->cc] = accept(me->socketServer->fd, (struct sockaddr *)&me->socketServer->address,  
                        (socklen_t*)&me->socketServer->al))<0)
            throw SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", "accept fail", me->socketServer->port, 0x61);
        #ifdef SOCKET_REPORTING
            SOCKET_REPORTING("New user connected\n");
        #endif
        me->connectionID = me->socketServer->cc;
        pthread_create(me->socketServer->threadStorage[me->socketServer->cc], &me->socketServer->attr, me->listener, arg);
        pthread_detach(*me->socketServer->threadStorage[me->socketServer->cc]);
        me->socketServer->cc++;
    }
}
void SocketServer::start(void *(*listener)(void *arg), int maxClients){
    this->arg = {this, listener};
    this->state = 1;
    pthread_create(&mainThread, &attr, this->listen, &arg);
    pthread_detach(mainThread);
}

/** socket Client interface **/
SocketClient::SocketClient(){
    Socket();
}
int SocketClient::connect(std::string addr, int port, int protocol){
    fd = socket(AF_INET, protocol, 0);
    address.sin_family = AF_INET; 
    address.sin_port = htons(port); 
    if(inet_pton(AF_INET, addr.c_str(), &address.sin_addr)<=0)
        throw(SocketException((char*)addr.c_str(), "Failed to connect to server: Host not resolved\
 or already in use. Failed on settings stage.", nullptr, port, -1));
    if (connectLS(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        throw(SocketException((char*)addr.c_str(), "Failed to connect to server: Host not resolved\
 or already in use. Failing in connection stage.", nullptr, port, -2));
    connection[0] = &fd;
    cc++;
}
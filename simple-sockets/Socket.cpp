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
int sideTo(int *array, int start, int end){
    for(int i = start; i < end; i++){
        array[i] = array[i+1];
    }
    end = 0;
}
int sideTo(int **array, int start, int end){
    for(int i = start; i < end -1; i++){
        array[i] = array[i+1];
    }
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
        throw SocketException("localhost", "Connection closed from other side", "discon", 60012, 0x71);
        //connection closerd
    }else if(sub < 0){
        throw SocketException("localhost", "Connection is exsistn't or busy", "notfound", 60012, 0x72);
        //connection already destroyed
    }
    else return std::string((char*)(buffer)); // connection active
}
std::string Socket::read(int conn){
    int sub = readLS(*connection[conn], buffer, this->bitrade);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    if(sub == 0){
        throw SocketException("localhost", "Connection closed from other side", "discon", 60012, 0x71);
        //connection closerd
    }else if(sub < 0){
        throw SocketException("localhost", "Connection is exsistn't or busy", "notfound", 60012, 0x72);
        //connection already destroyed
    }
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
int Socket::kill(int conn){
    delete (this->connection[conn]);
    sideTo(this->connection, conn, cc);
}
// int Socket::stop(int conn){

// }

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
    while(1){
        me->socketServer->connection[me->socketServer->cc] = new int(0);
        me->socketServer->threadStorage[me->socketServer->cc] = new pthread_t();
        if (listenLS(me->socketServer->fd, 3) < 0)
            throw SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", "Start failed", me->socketServer->port, 0x61);
        sleep(1);
        if ((*me->socketServer->connection[me->socketServer->cc] = accept(me->socketServer->fd, (struct sockaddr *)&me->socketServer->address,  
                        (socklen_t*)&me->socketServer->al))<0)
            throw SocketException((char*)me->socketServer->me.c_str(), "Failed for listening deamon.", "accept fail", me->socketServer->port, 0x61);
        #ifdef SOCKET_REPORTING
            SOCKET_REPORTING("New user connected\n");
        #endif
        me->connectionID = me->socketServer->cc;
        pthread_create(me->socketServer->threadStorage[me->socketServer->cc], &me->socketServer->attr, me->listener, new throwNetAttr(me, me->connectionID));
        pthread_detach(*me->socketServer->threadStorage[me->socketServer->cc]);
        me->socketServer->cc++;
    }
}
void SocketServer::start(void *(*listener)(void *arg), int maxClients){
    this->arg = {this, listener};
	this->arg.sockPtr = new ___SockPTR___uniQED__(___SockPTR___uniQED__(this, 1));
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

/*** NetworkListener interface **/

void NetworkListener::setStopCom(char com[3]){
    for(int i = 0; i < 3; i++)
        this->command[0][i] = com[i];
}
void NetworkListener::setStreanCom(char com[3]){
    for(int i = 0; i < 3; i++)
        this->command[1][i] = com[i];
}
void NetworkListener::setFileRequestCom(char com[3]){
    for(int i = 0; i < 3; i++)
        this->command[2][i] = com[i];
}

void NetworkListener::SetListener(int type, void (*listener)(void *attr)){
    if(type == DEFAULT_LISTENER)
        this->listeners.defaultListener = listener;
    else if(type == FILE_REQUEST_LISTENER)
        this->listeners.fileReaquestListener = listener;
    else if(type == STOP_LISTENER)
        this->listeners.stopListener = listener;
    else if(type == STREAM_LISTENER)
        this->listeners.streamListener = listener;
    else throw(SocketException((char*)"localhost/programm", (char*)"unknowen listener type", (char*)std::to_string(type).c_str(), 0x000, -9));
}

void NetworkListener::send(void *arg, char *data){
	netServerAttribures *attr = (netServerAttribures*)(arg);
	___SockPTR___uniQED__ *sockptr = getSockPointerFromAttr(attr);
    sockptr->send(data, sockptr->connn);
}
char *NetworkListener::read(void *arg){
	netServerAttribures *attr = (netServerAttribures*)(arg);
	___SockPTR___uniQED__ *sockptr = getSockPointerFromAttr(attr);
    return (char*)(sockptr->read(sockptr->connn)).c_str();
}
void NetworkListener::__Main__(void *arg){
	netServerAttribures *attr = (netServerAttribures*)(arg);
	___SockPTR___uniQED__ *sockptr = getSockPointerFromAttr(attr);
	this->listeners.defaultListener(arg);
}

std::string ___SockPTR___uniQED__::read(){
    readLS(*conn[*cc-1], this->buffer, *this->bitrade);
    if(buffer == NULL) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    return std::string((char*)(buffer));
}
std::string ___SockPTR___uniQED__::read(int conn){
    char *buf = this->buffer;
    if(readLS(*this->conn[conn], this->buffer, *this->bitrade)<0) printf("readLS is 0");
    this->buffer = buf;
    if(buffer == nullptr) throw(SocketException((char*)"Current serverd", (char*)"Failed to read incoming data, \
becouse it's is NULL", nullptr, 71, -1));
    std::string str = this->buffer;
    return str;
}
int ___SockPTR___uniQED__::send(std::string data){
    // try{

    // }catch()
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
}
Socket server and client library
GLOBUS-SOCKET v. 0.0.6
made by alexThunder (aka Rakutin Alexandr)
it's a simple library for sockets in linux
made with love <3

Classes:
Socket - base class, what need only for base operations. attributes in this class have in any classes (SocketServer, SocketClient)
SocketException - Class for exceptions, if socket connection failed
SocketServer - class for server. Have a few connections in "connection" array (pointers)
SocketClient - class for client. Have only one pointer to connaction
NetworkListener - class interface for listeners management
throwNetAttr - all information about connections, listeners and others
ListenerStream - Class-decoder for simple work with connection inside listener
___SockPTR__uniQED__ - pointers to basic socket's members. Do not use

Usage(functions):
Socket.read() - read from last connection
Socket.read(connection) - read from n'th connection
Socket.send(data) - send string to last connection
Socket.send(data, connection) - send string to n'th connaction
Socket.setBitrade(bitrade 1-10^6) - set count of max. read dyte count. if bitrade = 0, then it's throw exception. return current bitrade
Socket.setBuffer(pointer to buffer) - set buffer for reading. return pointer to current buffer.
SocketServer.bind(address, port) - bind server in address in port
SocketServer.start(pointer to request-manager, max. cout of clients) - start listening
SocketServer.listening(void *arg) - set custom listening function. Do not touch this, if you don't know low-level socket programming
SocketClient.connact(address, port) - connect client to server.
NetworkListener.setListener(type, pointer to listener) - set one listener.
NetworkListener.threadable(void*) - this funcion uses for throwing to pthread. This method manage all incoming requests and launch listeners
NetworkListener.setFileRequestCommand(char[3]) - (not released)
ListenerStream(void*) - decode listener arguments to simple structure
ListenerStream.send(data) - send data to current connection
ListenerStream.read() - read data from current connection
ListenerStream.close() - stop current connection
ListenerStream.setBuffer(buffer) - set buffer for reading from current connaction
ListenerStream.getCurrent() - get current connection ID
P.S.: types of entering data in .h file and functions declorations

Usage(variables):
Socket.buffer - pointer to reading buffer                                                                                                   (void *)
Socket.bitrade - max. bytes for package-reading                                                                                             (int)
Socket.address - low-level structure for connection
Socket.fd - file description of socket
Socket.connection - array of pointer to connections(if it's client it's just a pointer to main connection)
Socket.opt - options for low-level sockets
Socket.cc - last connection index
Socket.al - size of address structure
SocketException.address - address, where exception was called.
SocketException.description - description of called exception
SocketException.additional - additional information, if it's Have
SocketException.port - port, where exception was called
SocketException.codeOfError - if exception is not fatal, it's take code for stdSocketEncoder(in development)
SocketServer.throwenSocketServerStruct - structure for taking arguments for others threads(pthreads)
SocketServer.listen - function, what manage all incoming connections and register it's 
SocketServer.threadStorage - array of pthreads, what manage incoming data from connection and send answers
SocketServer.port - port of binded server
SocketServer.me - string address of server
SocketServer.mainThread - main thread, what launchs for listening of incoming connactions
SocketServer.attr - pthread attributes
SocketServer.state - condition, when server can works. If it's false, server stop all current connections and close they
throwNetAttr.connID - ID of current connection
throwNetAttr.listener - listener pointer
throwNetAttr.ptsd - pointer to full info about server/client
NetworkListener.command - commands for launching stream/file-transfer/stop modes
NetworkListener.listeners - struct with all listeners (private, and don't touch this)
NetworkListener.mainListener - ???
ListenerStream.attributes - (private, so don't touch this too) throwNetAttr object.
ListenerStream.buffer - pointer to reading buffer

Usage(structures)
SocketServer::throwenSocketServerStruct (aka netServerAttribures) is have:
    socketServer - pointer to current server object
    listener - pointer to low-level listener-function
    connectionID - current connection index
How to use in code:
    use fields for getting info about server. throwNetAttr replaced this structure as argument for function-pointer argument decoding
throwNetAttr is have:
    connID - current connection ID
    listener - listener for this event. if it's is nullptr, find function-pointer listener in ptsd
    ptsd - full info from server. If it's is nullptr, then use ptcd
    ptcd (in development) - full info about client deamon
How to use in code:
    in v0.0.5 examples you can see, what listening function declires with void pointer. pthread can't to see others, so you shoud to decode this pointer via this struct. How to do it:
    1) declire pointer to arguments (throwNetAttr *pointerToArguments)
    2) decode void* to netServerAttribures* ((throwNetAttr*)(argument))
    3) call structures members via pointerToArguments->member
    4) call something from current server via pointerToArguments->ptsd->socketServer->member
    5) done!

Defines:
    DEFAULT_LISTENER - uses in NetworkListener.setListener(type, ...). It's set default listener for all requests
    SOCKEXC_BROKENP - Broken pipe exception code
    STREAM_LISTENER - listener for big bitrades
    FILE_REQUEST_LISTENER - uses in NetworkListener.setListener(type, ...). It's set default listener for file-requests
    SOCKEXC_DISCONN - Disconnect exception code
    
What's new:
    ListenerStream::close() now remove not only heads of connection, but connection entity to.
    Apeend-New-Connection principle was replaced to Find-Null-and-Replace principle, so no trash at programm now.
    Fixed multi-user connections.
    Removed bug, when you try to read NULL connection SocketServer->sockPtr drops to 0s.
    Added first socked-based file transfer protocol, used FNODE system.
    Cleaners works better.
    Some framework's threads got a mutex now.


In development:
*Encoding and decoding classes - encode & decode incoming/outcoming message via algorithm
*File transfers protocols support
*HTTP support
*SSL support
*Listener integration for client's part
*
*replacing std::string to char[]
*compile need symbols of ".h" files to runtime-library and adding to runtime-library variant (./binlib)
*make same library for java, python, exordium

Compile with $g++ -c source.cpp -pthread -o executable.o

Tips:
    try to use NetworkListener class, not pointer-to-function listeners
    try to use lambda-functions in NetworkListener.setListener(type, [](void*){...});
    try to use only DEFAULT_LISTENER as type and use only this type. Other types in development (I have issiue about byte-command interface)
    try to make new reading buffers for all new connections
    try to use static reading buffers or allocate memory no less than bitrade
    try to write fast and good code
    
Library architecture:
    Giant part of library use pointers, so you want to edit code and after debug this... god bless you.
    Most functions and methods in library in beta-verions, so it's will be changed inside, but interface will be stay in this stage
    Я Панк и мне пофиг на потребление памяти, но пытаюсь оптимизировать. Если честно, то я совсем запутался в поинтерах и готов уже сдохнуть от них, а этот код считаю куском сами знаете чего. Но все-же буду его улучшать и оптимизировать
    

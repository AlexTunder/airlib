Socket server and client library
GLOBUS-SOCKET v. 0.0.1
made by alexThunder (aka Rakutin Alexandr)
it's a simple library for sockets in linux
made with love <3

Classes:
Socket - base class, what need only for base operations. attributes in this class have in any classes (SocketServer, SocketClient)
SocketException - Class for exceptions, if socket connection failed
SocketServer - class for server. Have a few connections in "connection" array (pointers)
SocketClient - class for client. Have only one pointer to connaction

Usage(functions):
Socket.read() - read from last connection
Socket.read(int connection) - read from n'th connection
Socket.send(data) - send string to last connection
Socket.send(data, connection) - send string to n'th connaction
Socket.setBitrade(bitrade 1-10^6) - set count of max. read dyte count. if bitrade = 0, then it's throw exception. return current bitrade
Socket.setBuffer(pointer to buffer) - set buffer for reading. return pointer to current buffer.
SocketServer.bind(address, port) - bind server in address in port
SocketServer.start(pointer to request-manager, max. cout of clients) - start listening
SocketServer.listening(void *arg) - set custom listening function. Do not touch this, if you don't know low-level socket programming
SocketClient.connact(address, port) - connect client to server.

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


Usage(structures)
SocketServer::throwenSocketServerStruct (aka netServerAttribures) is have:
    socketServer - pointer to current server object
    listener - pointer to low-level listener-function
    connectionID - current connection index
How to use in code:
    in example you can see, what listening function declires with void pointer. pthread can't to see others, so you shoud to decode this pointer. Ho to do it:
    1) declire pointer to arguments (netServerAttribures *pointerToArguments)
    2) decode void* to netServerAttribures* ((netServerAttribures*)(argument))
    3) call structures members via pointerToArguments->member
    4) call something from current server via pointerToArguments->socketServer->member
    5) done!

In development:
*Encoding and decoding classes - encode & decode incoming/outcoming message via algorithm
*Socket.kill(connection) - force destroing connection
*Socket.close(connection) - stopping connection
*Socket.sleep(connection) - stop connection for some time
*Socket.setActive(connection) - resume dialog with connection
*addConnection(connection) - add new proxy-type connection using anonimusEncodeProxy
*anonimusEncode(message, password) - function for encode message with password
*anonimusDecode(message, password) - function for decode message with password
*"anonimus" coding system
*removing std::string and others non-usable structures and libraries
*compile need symbols of ".h" files to runtime-library and adding to runtime-library variant
*make same library for java, python, c, exodium
*make listener class - class for comfortable listening function writing

Compile with $g++ -c source.cpp -pthread -o executable.o
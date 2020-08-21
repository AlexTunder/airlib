#include "Socket.hpp"
#include "./protocols/http.hpp"
#include <sys/stat.h>

SocketServer server;
HttpServerHandler http;

NetworkListener listener;

char *buffer;

int main(int argc, char **argv){
	//Install helpful data
	// if(argc<2){
	// }else{
	// 	http.setRoot(argv[1]);
	// 	http.setErrorPath(argv[2]);
	// }
	//Set constant arguments
	http.setRoutineArg("Server", "Custom HTTP server");
	http.setRoutineArg("Host", "127.0.0.1");
	http.setRoutineArg("Allow", "GET, HEAD, CONNECT");
	http.setRoutineArg("Accept-Ranges", "bytes");
	http.setRoutineArg("Content-Type", "text/html;charset=utf-8");
	http.setRoot("/home/alexthunder");
	http.setErrorPath("/home/alexthunder/hentai/globus/OpenCppNet/protocols/error");
	//setup listener
	listener.setListener(UPLOADS_LISTENER, [](void *raw){
		ListenerStream stream(raw);
		char *readingBuffer = (char*)malloc(HTTP_BITRATE);
		try{
			printf("Current (I)connection: %i\n", stream.getCurrent());
			while(1){
				strcpy(readingBuffer, stream.read().c_str());
				printf("Incoming data:\n%s\n****end****\n", readingBuffer);
				http.fill(readingBuffer);
			}
		}catch(SocketException e){
			printf("Exception was cathed\nAddr:%s\nDescription:%s\nAdditional:\"%s\"\nPort:%i\nCode:%i\n", e.address, e.description, e.additional, e.port, e.codeOfError);
		}
	});
	listener.setListener(DOWNLOAD_LISTENER, [](void *raw){
		ListenerStream stream(raw);
		char *writeBuffer;
		try{
			printf("Current (O)connection: %i\n", stream.getCurrent());
			while(1){
				if(http.isReady()){
					char *tmp = http.requests.getValue("$target");
					if(!strcmp(tmp, "/"))
						http.requests.setValue("$target", "/index.html");
					writeBuffer = http.flush();
					char strtmp[16];
					sprintf(strtmp, "%li", strlen(http.answer.getContent()));
					http.answer.setValue("Content-Length", strtmp);
					writeBuffer = http.flush();
					stream.send(writeBuffer);
					printf("Send: %s\n", writeBuffer);
					free(writeBuffer);
				}
			}
		}catch(SocketException e){
			printf("Exception was cathed\nAddr:%s\nDescription:%s\nAdditional:\"%s\"\nPort:%i\nCode:%i\n", e.address, e.description, e.additional, e.port, e.codeOfError);
		}
	});
	try{
		server.setBitrade(HTTP_BITRATE);
		server.setBuffer(buffer);
		server.bind("127.0.0.1", 8000);
		server.start(&listener, 1024);
		while(1){
			
		}
	}catch (SocketException e){
		printf("Exception was cathed\nAddr:%s\nDescription:%s\nAdditional:%s\nPort:%i\nCode:%i\n", e.address, e.description, e.additional, e.port, e.codeOfError);
	}
}
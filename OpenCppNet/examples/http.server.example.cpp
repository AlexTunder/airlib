#include "Socket.hpp"
#include "./protocols/http.hpp"
#include <sys/stat.h>

SocketServer server;

HttpRequest ans_template;

NetworkListener listener;

char *bsf;
HttpRequest requests[1024];

int main(int argc, char **argv){
	//Setup template for answers
	ans_template.setValue("Server", "Custom HTTP server.");
	ans_template.setValue("Connection", "keep-alive");
	ans_template.setValue("Host", server.me.c_str());

	listener.setListener(UPLOADS_LISTENER, [](void *raw){
		ListenerStream stream(raw);
		char *readingBuffer = (char*)malloc(HTTP_BITRATE), *token = (char*)malloc(HTTP_BITRATE);
		HttpRequest request = ans_template;
		request.setVirtual("$virtual /o/oauth2/token", token); //set virtual file
		request.setVirtual("$virtual /oauth2.token", token);
		try{
			while(1){
				if(requests[stream.getCurrent()].getValue("$target") != NULL){
					
					requests[stream.getCurrent()].clear(3);
				}
			}
		}catch(SocketException e){

		}
	});
	listener.setListener(DOWNLOAD_LISTENER, [](void *raw){
		ListenerStream stream(raw);
		char *writeBuffer;
		try{
			while(1){

			}
		}catch(SocketException e){
			printf("Exception was cathed\nAddr:%s\nDescription:%s\nAdditional:\"%s\"\nPort:%i\nCode:%i\n", e.address, e.description, e.additional, e.port, e.codeOfError);
		}
	});
	try{
		server.setBitrade(HTTP_BITRATE);
		server.setBuffer(bsf);
		server.bind("127.0.0.1", 80);
		server.start(&listener, 1024);
		while(1){
			
		}
	}catch (SocketException e){
		printf("Exception was cathed\nAddr:%s\nDescription:%s\nAdditional:%s\nPort:%i\nCode:%i\n", e.address, e.description, e.additional, e.port, e.codeOfError);
	}
}
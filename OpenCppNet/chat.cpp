#include "Socket.hpp"
#include <string.h>

SocketServer server;
NetworkListener listener;

struct Chat{
	char *last;
	bool updates;
};

char *token;
Chat *chat = (Chat*)malloc(sizeof(Chat)*1024);

int main(int argc, char **argv){
	token = argv[1];
	listener.setListener(UPLOADS_LISTENER, [](void *raw){
		ListenerStream <int>stream(raw);
		try{
			while(*stream.mutualRes() == 0);
			while(1){
				if(chat[*stream.mutualRes() - 1].updates){
					stream.send(chat[*stream.mutualRes() - 1].last);
					chat[*stream.mutualRes() - 1].updates = 0;
				}
			}
		}catch (SocketException e){

		}
	});
	listener.setListener(DOWNLOAD_LISTENER, [](void *raw){
		ListenerStream <int>stream(raw);
		stream.setBuffer((char*)(malloc(1024)));
		try{
			*stream.mutualRes() = atoi(stream.read().c_str()) + 1;
			stream.send(token);
			while(1){
				std::string buf = stream.read();
				chat[*stream.mutualRes() - 1].last = (char*)malloc(buf.length());
				strcpy(chat[*stream.mutualRes() - 1].last, buf.c_str());
				chat[*stream.mutualRes() - 1].updates = 1;
			}
		}catch (SocketException e){

		}
	});
}
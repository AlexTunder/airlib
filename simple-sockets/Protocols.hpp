#ifndef SCFD
#include "Socket.hpp"
#endif
//HTTP methods declires
#define HTTP_GET            0b0100
#define HTTP_POST           0b1011
#define HTTP_OPTIONS        0b0010
#define HTTP_HEAD           0b0110
#define HTTP_PUT            0b1100
#define HTTP_PATCH          0b1010
#define HTTP_DELETE         0b1111
#define HTTP_TRACE          0b0001
#define HTTP_CONNECT        0b0011
//JSON types ids declires
#define JSON_TPN_INT        0x61
#define JSON_TPN_DOUBLE     0x62
#define JSON_TPN_STRING     0x63
#define JSON_TPN_BOOLEAN    0x64
#define JSON_TPN_BINARY     0x65
#define JSON_TPN_CHAR       0x66
#define JSON_TPS_LINK       0x71
#define JSON_TPS_JSON       0x72
#define JSON_TPS_ARRAY      0x73
// #define HTTP

void strreadln(int index, char *from, char *to);

namespace netProtocol{
    //JSON form-support
    class st_json{
        private:
            char **field, **value;
        public:
            char *find(char *fieldName);
            char *operator[](int index);
            int   ifind(char *fieldName);
            void  remove(int index);
            void  move(int from, int to, bool replace = false);
            int   getType(int index); //return JSON_TP_(type) (0x61 - 0x66)
            void  set(char *full);
            char *get();
            void  push(char *fielname, char *value);
            st_json(char *str);
            st_json();
    };
    class s_json{
        private:
            st_json root;
            st_json *links;
    };
    //class json is unsupported yet
    //HTTP support
    class HttpClientRequest{
        public:
            //http-personal fields
            int requesType; char *file, *referer;
            char *acceptType; char *userAgent;
            bool closeConnection;
            char *host;
            //universal members
            void  set(char *full);
            st_json additional;
            char *get();
            void push(char *line);
            //content
            char *content;
    };
};

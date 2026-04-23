#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#include "../core/event.h"
#include <cJSON.h>
typedef struct http http;
typedef struct http_connection http_connection;
typedef enum http_method {
    HTTP_GET_REQUEST,
    HTTP_POST_REQUEST
} http_method;

create_event_category(networking,0);

typedef struct networking_recieve{
    event base;
    char* Recieved_Data;
    http_connection* Connection;
    size_t Size;
}networking_recieve;
create_event_type(networking_recieve,1);
void networking_recieve_init(networking_recieve *Event,http_connection* Connection,char* Recieved_Data,size_t Size);

typedef struct networking_new_connection{
    event base;
    http_connection* Connection;
    char Ip[64];
    uint16_t Port;
}networking_new_connection;
create_event_type(networking_new_connection,0);
void networking_new_connection_init(networking_new_connection *Event,http_connection *Connection,char Ip[], uint16_t Port);

typedef struct http_response http_response;
typedef struct http_request http_request;
http *create_http_server(const char* address,uint16_t port);
http *create_http_client(void);
http_connection *connect_http_client(http* client,char address[],uint16_t port);
void poll_http(http* server);
void destroy_http_server(http* server);

inline static void send_http_request_json(http_connection* Connection,http_method Method,cJSON* Body_JSON, const char* Path,const char* Headers);
void send_http_request(http_connection* Connection,http_method Method,char* Body, const char* Path,const char* Headers);


inline static void send_http_response_json(http_connection* Connection,cJSON *Body_JSON, int Status_Code, const char* Status_Text,const char* Headers);
void send_http_response(http_connection* Connection,const char *Body, int Status_Code, const char* Status_Text,const char* Headers);

#endif
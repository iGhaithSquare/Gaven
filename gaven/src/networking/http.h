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

typedef struct networking_poll{
    event base;
}networking_poll;
create_event_type(networking_poll,0);
void networking_poll_init(networking_poll *Event);

typedef struct http_response http_response;
typedef struct http_request http_request;
http *create_http_server(const char* address,int port);
http *connect_http_client(const char* address,int port);
void poll_http(http* server);
void destroy_http_server(http* server);


void send_http_request(http_connection* Connection,http_method Method,cJSON *Body_JSON, const char* Path,const char* Headers);
/* Creates an http request doesnt send it.*/
/* To Send use send_http_request */
http_request create_http_request(http_method Method, cJSON *Body_JSON, const char* Path,const char* Headers);
void destroy_http_request(http_request* Request);
void send_as_http_request(http_connection* Connection, const http_request* Request);

inline static void send_http_response_json(http_connection* Connection,cJSON *Body_JSON, int Status_Code, const char* Status_Text,const char* Headers);
void send_http_response(http_connection* Connection,const char *Body, int Status_Code, const char* Status_Text,const char* Headers);

#endif
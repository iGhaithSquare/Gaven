#include "core/debug/log.h"
#include <stdio.h>
#include "http.h"
#include <stdint.h>
#include <cJSON.h>
#define HTTP_BACKLOG 20
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET net_socket;
#else
#include <sys/socket.h>
#include <sys/netdb.h>
#include <errno.h>
typedef int net_socket;
#endif
void application_event_callback(event *e);
typedef struct http_stream {
    char* Data;
    size_t Pos;
    size_t Size;
    size_t Cap;
    size_t Content_Length;
} http_stream;
struct http_connection {
    http_stream Recieving_Stream;
    http_stream Sending_Stream;
    net_socket Socket;
    uint8_t Verified;

    char IP[64];
    uint16_t Port;
};

struct http{
    struct addrinfo *Res;
    net_socket Socket;
    size_t Connections_Count;
    size_t Connections_Size;
    http_connection **Connections;
};
struct http_request{
    http_method Method;
    const char* Path;
    char* Headers;
    char* Body;
};
struct http_response{
    uint32_t Status_Code;
    const char* Status_Text;
    char* Headers;
    const char* Body;
};
static inline void set_nonblocking(net_socket s){
    #ifdef _WIN32
    long mode =1;
    ioctlsocket(s,FIONBIO,&mode);
    #else
    int flags=fcntl(s,F_GETFL,0);
    fcntl(s,F_SETFL,flags | O_NONBLOCK);
    #endif
}
void resolve_address(const char* ip_address,int port,http *server){
    char port_c[8];
    snprintf(port_c,sizeof(port_c),"%d",port);
    int status;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    if(ip_address == NULL) hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    status = getaddrinfo(ip_address, port_c, &hints, &server->Res);
    GAVEN_ASSERT(status==0,"gai error: %s\n", gai_strerror(status));
}
void start_windows_networkthingy(void){
    static int A = 0;
    WSADATA wsaData;
    if(A) return;

    GAVEN_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsaData)==0,"WSAStartup failed.");
    if (LOBYTE(wsaData.wVersion) != 2 ||HIBYTE(wsaData.wVersion) != 2){
        WSACleanup();
        GAVEN_ASSERT(0,"Version 2.2 of Winsock not available.");
    }
    A=1;
}
http* make_valid_http_context(void){
    http* Ctx= (http*)malloc(sizeof(http));
    GAVEN_ASSERT(Ctx,"Couldnt create networking context");
    Ctx->Connections=NULL;
    Ctx->Connections_Count=0;
    Ctx->Connections_Size=0;
    Ctx->Res=NULL;
    #ifdef _WIN32
    Ctx->Socket=INVALID_SOCKET;
    #else
    Ctx->Socket=-1;
    #endif
    return Ctx;
}
http *create_http_server(const char* address,uint16_t port){
    #ifdef _WIN32
    start_windows_networkthingy();
    #endif
    http *server = make_valid_http_context();
    
    #ifdef _WIN32
    char yes =1;
    #else
    int yes =1;
    #endif
    struct addrinfo* p;
    resolve_address(address,(int)(port),server);
    for(p = server->Res; p != NULL; p = p->ai_next) {
        server->Socket = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
        if (server->Socket==-1) continue;
        set_nonblocking(server->Socket);
        setsockopt(server->Socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(server->Socket, p->ai_addr, p->ai_addrlen) == -1) {
            #ifdef _WIN32
                closesocket(server->Socket);
            #else
                close(server->Socket);
            #endif
            continue;
        }
        break;
    }
    listen(server->Socket,HTTP_BACKLOG);
    freeaddrinfo(server->Res);
    return server;
}
uint8_t check_for_connection(http* server,net_socket Socket){
    if(server->Connections_Size!=0){
        for(int i=0;i<server->Connections_Count;i++){
            if(server->Connections[i]->Socket==Socket)
                return 1;
        }
    }
    return 0;
}
http_connection* create_connection(http* server, net_socket Socket, char host[],uint16_t port){
    if(server->Connections_Size==0){
        server->Connections_Count=0;
        server->Connections_Size=4;
        server->Connections=(http_connection**)malloc(sizeof(http_connection*)*server->Connections_Size);
        http_connection* con;
        con = malloc(sizeof(http_connection));
        memset(con,0,sizeof(http_connection));
        con->Socket=Socket;
        strncpy(con->IP,host,sizeof(con->IP)-1);
        con->Port=port;
        con->Verified=0;
        server->Connections[server->Connections_Count++]=con;
        return con;
    }
    size_t new_cap = server->Connections_Size;
    if(new_cap<=server->Connections_Count)
        new_cap=new_cap*2;
    if(new_cap!=server->Connections_Size){
        http_connection** temp = realloc(server->Connections,new_cap*sizeof(http_connection*));
        GAVEN_ASSERT(temp, "Couldnt allocate new memory");
        server->Connections_Size = new_cap;
        server->Connections = temp;
    }
    http_connection* con;
    con = malloc(sizeof(http_connection));
    memset(con,0,sizeof(http_connection));
    con->Socket=Socket;
    strncpy(con->IP,host,sizeof(con->IP)-1);
    con->Port=port;
    con->Verified=0;
    server->Connections[server->Connections_Count++]=con;
    return con;
}
http *create_http_client(void){
    #ifdef _WIN32
    start_windows_networkthingy();
    #endif
    http *client = (http*)calloc(1,sizeof(http));
    #ifdef _WIN32
    client->Socket=INVALID_SOCKET;
    #else
    client->Socket=-1;
    #endif
    return client;
}
http_connection *connect_http_client(http* client,char address[],uint16_t port){
    uint8_t connected =0;
    struct addrinfo* p;
    net_socket Socket;
    resolve_address(address,(int)(port),client);
    for(p = client->Res; p != NULL; p = p->ai_next) {
        Socket = socket(p->ai_family, p->ai_socktype,p->ai_protocol);
        if (Socket==-1) continue;
        set_nonblocking(Socket);
        int result=connect(Socket, p->ai_addr, p->ai_addrlen);
        #ifdef _WIN32
        if (result ==0||(result ==SOCKET_ERROR && WSAGetLastError()==WSAEWOULDBLOCK)) {
            connected=1;
            break;
        }    
        closesocket(Socket);
        #else
        if (result ==0||(result ==-1 &&errno==EINPROGRESS)) {
            connected=1;
            break;
        }
        close(Socket);
        #endif
    }
    http_connection* con=NULL;
    if(connected&&!check_for_connection(client,Socket)){
        con=create_connection(client,Socket,address,port);
    } else GAVEN_WARN("Couldnt Connect Client");
    freeaddrinfo(client->Res);
    return con;
}

void http_stream_append_data(http_stream* Stream, const char* data,size_t length){
    if(Stream->Cap<=Stream->Size+length){
        size_t new_cap = (Stream->Cap?Stream->Cap:1024);
        do{ new_cap*=2;}while(new_cap<=Stream->Size+length);
        char *new_data = realloc(Stream->Data,new_cap);
        GAVEN_ASSERT(new_data,"Failed to Add data to stream");
        Stream->Data=new_data;
        Stream->Cap=new_cap;
    }
    memcpy(Stream->Data+Stream->Size,data,length);
    Stream->Size+=length;
}
void send_http_step(http_connection *Connection){
    http_stream *Send_Stream = &Connection->Sending_Stream;
    int Send_Chunk  = send(Connection->Socket,Send_Stream->Data,Send_Stream->Size,0);
    if(Send_Chunk > 0){
        size_t Remaining = Send_Stream->Size-(size_t)(Send_Chunk);
        if (Remaining > 0) memmove(Send_Stream->Data,Send_Stream->Data+(size_t)(Send_Chunk),Remaining);
        Send_Stream->Size=Remaining;
        return;
    }
    #ifdef _WIN32
    int err = WSAGetLastError();
    if (err==WSAEWOULDBLOCK) return;
    #else
    int err = errno;
    if (err==EAGAIN||err==EWOULDBLOCK) return;
    #endif
    GAVEN_WARN("Sending http failed: %d",err);
}
void packet_recieved(http_connection *Connection){
    http_stream *Recieve_Stream = &Connection->Recieving_Stream;
    char *Packet_Data = malloc(Recieve_Stream->Pos);
    memcpy(Packet_Data,Recieve_Stream->Data,Recieve_Stream->Pos);
    networking_recieve Recieve_Event;
    networking_recieve_init(&Recieve_Event,Connection,Packet_Data,Recieve_Stream->Pos);
    application_event_callback(&Recieve_Event.base);
    memmove(Recieve_Stream->Data,Recieve_Stream->Data+Recieve_Stream->Pos,Recieve_Stream->Size-Recieve_Stream->Pos);
    Recieve_Stream->Size-=Recieve_Stream->Pos;
    Recieve_Stream->Pos=0;
    Recieve_Stream->Content_Length=0;
    free(Packet_Data);
}
void parse_http_stream_header(http_stream *Recieve_Stream){
    size_t i=0;
    const char* Data= Recieve_Stream->Data;
    size_t length=0;
    while(i+14<Recieve_Stream->Pos){
        if(Data[i]=='C'&&Data[i+1]=='o'&&Data[i+2]=='n'&&Data[i+3]=='t'&&Data[i+4]=='e'&&Data[i+5]=='n'&&Data[i+6]=='t'&&Data[i+7]=='-'&&Data[i+8]=='L'&&Data[i+9]=='e'&&Data[i+10
        ]=='n'&&Data[i+11]=='g'&&Data[i+12]=='t'&&Data[i+13]=='h'&&Data[i+14]==':'){
            i+=15;
            while(Data[i]==' '||Data[i]=='\t')
                i++;
            while(Data[i]<='9'&&Data[i]>='0'){
                length*=10;
                length+=(size_t)(Data[i]-'0');
                i++;
            }
            Recieve_Stream->Content_Length=length;
            return;
        }
        i++;
    }
    Recieve_Stream->Content_Length=0;
    return;
}
void recieve_http_step(http *Http,http_connection *Connection){
    size_t i;
    http_stream* Recieve_Stream = &Connection->Recieving_Stream;
    if(Recieve_Stream->Cap==0){
        Recieve_Stream->Cap=1024;
        char* temp = realloc(Recieve_Stream->Data,Recieve_Stream->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory for the recieve stream");
        Recieve_Stream->Data = temp;
        Recieve_Stream->Content_Length=0;
        Recieve_Stream->Pos=0;
        Recieve_Stream->Size=0;
    }
    else if(Recieve_Stream->Size>=Recieve_Stream->Cap){
        Recieve_Stream->Cap*=2;
        char* temp = realloc(Recieve_Stream->Data,Recieve_Stream->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory for the recieve stream");
        Recieve_Stream->Data = temp;
    }
    int Recieve_Chunk  = recv(Connection->Socket,Recieve_Stream->Data+Recieve_Stream->Size,Recieve_Stream->Cap-Recieve_Stream->Size,0);
    if(Recieve_Chunk > 0){
        Recieve_Stream->Size+=(size_t)(Recieve_Chunk);
        while(Recieve_Stream->Pos+3<Recieve_Stream->Size){
            if(Recieve_Stream->Data[Recieve_Stream->Pos]=='\r'&&Recieve_Stream->Data[Recieve_Stream->Pos+1]=='\n'&&Recieve_Stream->Data[Recieve_Stream->Pos+2
            ]=='\r'&&Recieve_Stream->Data[Recieve_Stream->Pos+3]=='\n'){
                if(!Recieve_Stream->Content_Length)
                    parse_http_stream_header(Recieve_Stream);
                if(Recieve_Stream->Pos+Recieve_Stream->Content_Length+3>=Recieve_Stream->Size)return;
                Recieve_Stream->Pos+=Recieve_Stream->Content_Length+4;
                packet_recieved(Connection);
                Recieve_Stream->Content_Length=0;
            }
            else
                Recieve_Stream->Pos++;
        }
        return;
    }
    if(Recieve_Chunk==0){
        // client disconnected
        for(i=0;i<Http->Connections_Count;i++)
            if(Http->Connections[i]==Connection){
                Http->Connections[i]=Http->Connections[Http->Connections_Count-1];
                free(Http->Connections[i]);
                Http->Connections_Count--;
                return;
            }
    }
    #ifdef _WIN32
    int err = WSAGetLastError();
    if (err==WSAEWOULDBLOCK) return;
    #else
    int err = errno;
    if (err==EAGAIN||err==EWOULDBLOCK) return;
    #endif
    GAVEN_WARN("Recieving packed failed: %d",err);

}
void poll_http(http* server){
    size_t i;
    struct sockaddr_storage Their_Addr;
    #ifdef _WIN32 //The server socket is defined means this is a server not a client so we can accept.
    if(server->Socket!=INVALID_SOCKET){
    #else
    if(server->Socket!=0){
    #endif
        while(1){
            socklen_t address_size = sizeof(Their_Addr);
            net_socket clientfd = accept(server->Socket, (struct sockaddr *)&Their_Addr,&address_size);
            if( clientfd !=-1){
                char host[64];
                char port_c[8];
                uint16_t port;
                socklen_t len = sizeof(Their_Addr);
                getnameinfo((struct sockaddr*)&Their_Addr,len,host,64,port_c,8,NI_NUMERICHOST|NI_NUMERICSERV);
                port = (uint16_t)(atoi(port_c));
                http_connection* con =create_connection(server,clientfd,host,port);
                networking_new_connection New_Con;
                networking_new_connection_init(&New_Con,con,host,port);
                application_event_callback(&New_Con.base);
            }
            else{
                #ifdef _WIN32
                int err= WSAGetLastError();
                if(err!=WSAEWOULDBLOCK){
                    GAVEN_WARN("Polling Error: %d",err);
                    //error event callback
                }
                else break;
                #else
                if((errno!=EAGAIN)&&(errno!=EWOULDBLOCK)){
                    GAVEN_WARN("Polling Error: %d",errno);
                    //error event callback
                }
                else break;
                #endif
            }
        }
    }
    
    for(i=0;i<server->Connections_Count;i++){
        http_connection *Connection = server->Connections[i];
        if(Connection->Verified==0){
            fd_set wfd;
            FD_ZERO(&wfd);
            FD_SET(Connection->Socket,&wfd);
            struct timeval tv = {0,0};
            int r = select(Connection->Socket+1,NULL,&wfd,NULL,&tv);
            int err = 0;
            if(r>0){
                socklen_t lens = sizeof(err);
                getsockopt(Connection->Socket,SOL_SOCKET,SO_ERROR,(char*)&err,&lens);
                if(err == 0){
                    GAVEN_INFO("CONNECTED");
                    Connection->Verified=1;
                }
                //todo: add timeout
                else{
                #ifdef _WIN32
                    closesocket(Connection->Socket);
                #else
                    close(Connection->Socket);
                #endif
                    server->Connections[i]=server->Connections[server->Connections_Count-1];
                    free(Connection);
                    server->Connections_Count--;
                    i--;
                    GAVEN_WARN("Connection Failed with error: %d",err);
                    continue;
                }
            }
            else if(r<0){
                #ifdef _WIN32
                err=WSAGetLastError();
                closesocket(Connection->Socket);
                #else
                err=errno;
                close(Connection->Socket);
                #endif
                server->Connections[i]=server->Connections[server->Connections_Count-1];
                free(Connection);
                server->Connections_Count--;
                i--;
                GAVEN_WARN("Connection Failed with error: %d",err);
                continue;
            }
            else
                continue;
        }
        recieve_http_step(server,Connection);
        if(Connection->Sending_Stream.Size>0)
            send_http_step(Connection);
    }
}
int send_http(net_socket Socket,char* buf, size_t* len){
    size_t total = 0;
    int n;
    size_t remaining = *len;
    int chunk=(remaining>INT_MAX)?INT_MAX:(int)(remaining);
    while(total < *len) {
        n = send(Socket, buf+total, chunk, 0);
        
        #ifdef _WIN32
        if (n==SOCKET_ERROR){
            int err = WSAGetLastError();
            GAVEN_WARN("Sending packed failed: %d",err);
            return -1;
        }
        #else
        if (n == -1) {
            int err = errno;
            GAVEN_WARN("Sending packed failed: %d",err);
            return -1;
        }
        #endif
        if( n==0){
            GAVEN_WARN("Connection closed");
            return -1;
        }
        total += n;
        remaining=*len-total;
        chunk=(remaining>INT_MAX)?INT_MAX:(int)(remaining);
    }
    *len = total;
    return 0;
}
http_request create_http_request(http_method Method,char *Body, const char* Path,const char* Headers){
    char *h;
    if(!Headers) {
        h=malloc(3);
        h[0] ='\r';
        h[1] ='\n';
        h[2]='\0';
    }
    else{
        size_t len =strlen(Headers);
        h=malloc(len+3);
        strcpy(h,Headers);
        if(h&&h[0]!='\0'&&(len<2||h[len-2]!='\r'||h[len-1]!='\n')){
            h[len]= '\r';
            h[len+1]= '\n';
            h[len+2] = '\0';
        }
    }
    http_request Req = {
        .Method=Method,
        .Body=(Body?Body:""),
        .Path=(Path?Path:"/"),
        .Headers=h
    };
    return Req;
}
void send_as_http_request(http_connection* Connection, const http_request* Request){
    size_t total_size;
    size_t body_length = Request->Body?strlen(Request->Body):0;
    char c_body_length[64];
    const char* method_str = (Request->Method==HTTP_GET_REQUEST)? "GET" :(Request->Method==HTTP_POST_REQUEST)? "POST" : "UNKNOWN"; 
    snprintf(c_body_length,64,"%zu",body_length);
    total_size = strlen(method_str)+1+strlen(Request->Path)+11+strlen(Request->Headers)+16+strlen(c_body_length)+4+body_length;
    char* buffer=malloc(total_size+1);
    GAVEN_ASSERT(buffer!=NULL,"Failed to create http request buffer");
    snprintf(buffer,total_size+1,"%s %s HTTP/1.1\r\n%sContent-Length: %s\r\n\r\n%s",method_str,Request->Path,Request->Headers,c_body_length,Request->Body);
    http_stream_append_data(&Connection->Sending_Stream,buffer,total_size);
    free(buffer);
}
void destroy_http_request(http_request* Request){
    if(!Request) return;
    free(Request->Headers);
}

inline static void send_http_request_json(http_connection* Connection,http_method Method,cJSON* Body_JSON, const char* Path,const char* Headers){
    char* Body=cJSON_PrintUnformatted(Body_JSON);
    send_http_request(Connection,Method,Body,Path,Headers);
    free(Body);
}
void send_http_request(http_connection* Connection,http_method Method,char* Body, const char* Path,const char* Headers){
    http_request h = create_http_request(Method,Body,Path,Headers);
    send_as_http_request(Connection,&h);
    destroy_http_request(&h);
}

// response
http_response create_http_response(const char *Body, int Status_Code, const char* Status_Text,const char* Headers){
    char *h;
    if(!Headers) {
        h=malloc(3);
        GAVEN_ASSERT(h,"Couldnt Allocated Memory for header");
        h[0] ='\r';
        h[1] ='\n';
        h[2]='\0';
    }
    else{
        size_t len =strlen(Headers);
        h=malloc(len+3);
        GAVEN_ASSERT(h,"Couldnt Allocated Memory for header");
        strcpy(h,Headers);
        if(h&&h[0]!='\0'&&(len<2||h[len-2]!='\r'||h[len-1]!='\n')){
            h[len]= '\r';
            h[len+1]= '\n';
            h[len+2] = '\0';
        }
    }
    http_response Req = {
        .Status_Code=Status_Code,
        .Body=(Body?Body:""),
        .Status_Text=(Status_Text?Status_Text:"OK"),
        .Headers=h
    };
    return Req;
}
void send_as_http_response(http_connection* Connection, http_response* Response){
    size_t total_size;
    size_t body_length = Response->Body?strlen(Response->Body):0;
    char c_body_length[64];
    snprintf(c_body_length,64,"%zu",body_length);
    uint32_t status_code_length=0;
    uint32_t i =Response->Status_Code;
    do{i/=10; status_code_length++;} while (i);
    total_size = 9+status_code_length+1+strlen(Response->Status_Text)+2+strlen(Response->Headers)+16+strlen(c_body_length)+4+body_length;
    char* buffer=malloc(total_size+1);
    GAVEN_ASSERT(buffer!=NULL,"Failed to create http request buffer");
    snprintf(buffer,total_size+1,"HTTP/1.1 %d %s\r\n%sContent-Length:%zu\r\n\r\n%s",Response->Status_Code,Response->Status_Text,Response->Headers,body_length,Response->Body);
    http_stream_append_data(&Connection->Sending_Stream,buffer,total_size);
    free(buffer);
}
void destroy_http_response(http_response* Response){
    if(!Response) return;
    free(Response->Headers);
}
inline static void send_http_response_json(http_connection* Connection,cJSON *Body_JSON, int Status_Code, const char* Status_Text,const char* Headers){
    char *Body=cJSON_PrintUnformatted(Body_JSON);
    send_http_response(Connection,Body,Status_Code,Status_Text,Headers);
    free(Body);
}
void send_http_response(http_connection* Connection,const char *Body, int Status_Code, const char* Status_Text,const char* Headers){
    http_response h = create_http_response(Body,Status_Code,Status_Text,Headers);
    send_as_http_response(Connection,&h);
    destroy_http_response(&h);
}
void destroy_http_server(http* server){
    #ifdef _WIN32
        closesocket(server->Socket);
    #else
        close(server->Socket);
    #endif
    if(server->Res!=NULL)
        freeaddrinfo(server->Res);
}



static inline void networking_new_connection_to_string(event *Event, char* buffer, size_t buffer_size){
    if (!buffer) return;
    networking_new_connection *Ev = (networking_new_connection *) Event;
    snprintf(buffer, buffer_size, "New connection at %s Port: %d",Ev->Ip,Ev->Port);
}
void networking_new_connection_init(networking_new_connection *Event,http_connection *Connection,char Ip[], uint16_t Port){
    if(!Event) return;
    Event->base.Category_Flags = event_category_networking;
    Event->base.Handled = 0;
    Event->base.Name = "Networking New Connection";
    Event->Connection = Connection;
    strncpy(Event->Ip,Ip,64-1);
    Event->Port=Port;
    Event->base.To_String = networking_new_connection_to_string;
    Event->base.Type = event_type_networking_new_connection;
}

static inline void networking_recieve_to_string(event *Event, char* buffer, size_t buffer_size){
    if (!buffer) return;
    networking_recieve *Poll = (networking_recieve *) Event;
    snprintf(buffer, buffer_size, "Packed Recieved: %.*s",Poll->Size, Poll->Recieved_Data);
}

void networking_recieve_init(networking_recieve *Event,http_connection* Connection,char* Recieved_Data,size_t Size){
    if(!Event) return;
    Event->base.Category_Flags = event_category_networking;
    Event->base.Handled = 0;
    Event->base.Name = "Networking Packet Recieved";
    Event->base.To_String = networking_recieve_to_string;
    Event->base.Type = event_type_networking_recieve;
    Event->Connection= Connection;
    Event->Recieved_Data = Recieved_Data;
    Event->Size = Size;
}
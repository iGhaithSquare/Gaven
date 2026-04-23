#include "gaven.h"
#include <stdio.h>
#include <stdlib.h>
//todo: fix memory leaks.
/*creating a layer phase*/
create_layer_phase(Update,0);
/* creating layer data*/
typedef struct example_layer_data{
    http* Server;
    http* Client;
    http_connection *Client_Connection;
} example_layer_data;
/* using OnEvent */
int example_on_networking_recieve(networking_recieve* E){
    const char* headers =
    "Content-Type: text/html; charset=utf-8\r\n";

    const char* body =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>Gaven Server</title>"
    "</head>"
    "<body>"
    "<h1>Hello World</h1>"
    "<p>Your server is working.</p>"
    "</body>"
    "</html>";
    GAVEN_INFO("Data recieved: %.*s",1028,E->Recieved_Data);
    send_http_response(E->Connection, body, 200, "OK", headers);
    return 1;
}
void example_on_event(layer* self, event* Event){
    char buffer[1024];
    Event->To_String(Event,buffer,1024);
    EVENT_DISPATCH(Event,networking_recieve,example_on_networking_recieve);
} 
/* detaching */
void example_on_dettach(layer*self){
    /*using layer data*/
    example_layer_data* Data = (example_layer_data*)self->LayerData;
    destroy_http_server(Data->Server);
    destroy_http_server(Data->Client);
}
/*creating a layer callback*/
void polling_callback(layer* self, void* ctx){
    /*using layer data*/
    example_layer_data* Data = (example_layer_data*)self->LayerData;
    poll_http(Data->Server);
    poll_http(Data->Client);
}
/* creating an event category */
create_event_category(test_category,0);
/* creating an event type */
typedef struct test_event{
    event base;
    int value;
}test_event;
create_event_type(test_event,0);
static inline void test_event_to_string(event *Event, char* buffer, size_t buffer_size){
    if (!buffer) return;
    test_event *Test_Event = (test_event *) Event;
    snprintf(buffer, buffer_size, "Test Event: Value = %d",Test_Event->value);
}
static inline void test_event_init(test_event *Event, int value){
    if(!Event) return;
    Event->base.Category_Flags = event_category_test_category;
    Event->base.Handled = 0;
    Event->base.Name = "Test Event";
    Event->base.To_String = test_event_to_string;
    Event->base.Type = event_type_test_event;
    Event->value = value;

}

/* The user can either use the prebuilt main or create his own*/
/* to create our own main we first define NO_GAVEN_MAIN */
/* Using our own main*/
#ifdef NO_GAVEN_MAIN
int main(){
    /* We create the application*/
    application* app = create_gaven_application();
    /* We set the running to true*/
    app->running=1;
    /* User Specific Code */
    GAVEN_WARN("HELLO WORLD");
    /* We call the run loop */
    run_application();
    /* We destroy the application*/
    destroy_application();
    return 0;
}
#else
/* To use the prebuilt main we dont define anything */
/* Using the prebuilt Gaven Main workflow */
application* gaven_main(int argc, char** argv){
    /* We create the application*/
    application* app = create_gaven_application();
    /* User Specific Code */
    GAVEN_WARN("Warn0");
    /* Creating A layer*/
    layer* example_layer=calloc(1,sizeof(layer));
    /*binding the layer to a phase*/
    bind_layer_phase(example_layer,layer_phase_polling,polling_callback,NULL);
    /* using layer data */
    example_layer_data *data = (example_layer_data*)malloc(sizeof(example_layer_data));
    /* Creates an online server */
    data->Server = create_http_server(NULL,5000);
    /* Creates an online client*/
    data->Client = create_http_client();
    send_http_request(connect_http_client(data->Client,"httpbin.org",80),HTTP_GET_REQUEST,"","/json","Host: example.com\r\nConnection: close\r\n");
    GAVEN_WARN("AFTER REQUEST");
    example_layer->OnEvent=example_on_event;
    example_layer->LayerData = data;
    /* Adding the layer to the registry */
    add_layer(app->Layer_Registry,example_layer);
    test_event TEST_EVENT;
    test_event_init(&TEST_EVENT,1);
    char test_event_print[64];
    TEST_EVENT.base.To_String(&(TEST_EVENT.base),test_event_print,64);
    GAVEN_WARN("%s",test_event_print);
    /* We return the application*/
    return app;
}
#endif
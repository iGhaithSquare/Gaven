#include "gaven.h"
#include <stdio.h>
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
    run_application(app);
    /* We destroy the application*/
    destroy_application(app);
    return 0;
}
#else
/* To use the prebuilt main we dont define anything */
/* Using the prebuilt Gaven Main workflow */
application* gaven_main(int argc, char** argv){
    /* We create the application*/
    application* app = create_gaven_application();
    /* User Specific Code */
    GAVEN_WARN("HELLO WORLD");
    GAVEN_INFO("ANOTHER THING TO BE PRINTED");
    
    test_event TEST_EVENT;
    test_event_init(&TEST_EVENT,1);
    char test_event_print[64];
    TEST_EVENT.base.To_String(&(TEST_EVENT.base),test_event_print,64);
    GAVEN_WARN("%s",test_event_print);
    /* We return the application*/
    return app;
}
#endif
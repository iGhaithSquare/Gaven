#include "application.h"
#include "malloc.h"
application* create_gaven_application(){
    application* app = (application*)malloc(sizeof(application));
    app->running=1;
    return app;
}

void run_application(application* self){
    while(self->running);
}
void destroy_application(application* self){
    if(!self) return;
    free(self);
}
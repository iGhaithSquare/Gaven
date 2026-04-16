#include "application.h"
#include <stdlib.h>
#include "layer/layer_registry.h"
application* create_gaven_application(){
    application* app = (application*)malloc(sizeof(application));
    app->Layer_Registry = create_layer_registry();
    app->Running=1;
    return app;
}
void run_application(application* self){
    size_t i,j;
    if(self->user_pump_events_function)
        self->user_pump_events_function(self);
    layer_registry *Layer_Registry = self->Layer_Registry;
    while(self->Running){
        layer_registry *Registry =self->Layer_Registry;
        for (i=0;i<Registry->Phase_Count;i++){
            phase_bucket *Bucket = &Registry->Phase_Buckets[i];
            for(j=0;j<Bucket->Count;j++){
                layer_binding* b =Bucket->Bindings[j];
                b->Layer_phase_callback(b->Layer,b->Phase_Context);
            }
        }
    };
}
void destroy_application(application* self){
    if(!self) return;
    destroy_layer_registry(self->Layer_Registry);
    destroy_logging();
    free(self);
}
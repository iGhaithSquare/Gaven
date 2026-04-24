#include "application.h"
#include <stdlib.h>
#include "layer/layer_registry.h"
static application* Application;
application* create_gaven_application(){
    Application = (application*)malloc(sizeof(application));
    Application->Layer_Registry = create_layer_registry();
    Application->Running=1;
    return Application;
}
void run_application(void){
    size_t i,j;
    layer_registry *Layer_Registry = Application->Layer_Registry;
    while(Application->Running){
        layer_registry *Registry =Application->Layer_Registry;
        for (i=0;i<Registry->Phase_Count;i++){
            phase_bucket *Bucket = &Registry->Phase_Buckets[i];
            for(j=0;j<Bucket->Count;j++){
                layer_binding* b =Bucket->Bindings[j];
                b->Layer_phase_callback(b->Layer,Bucket->Phase_Context);
            }
        }
    };
}
void application_event_callback(event *e){
    if(!e) return;
    size_t i;
    layer_registry* Registry =Application->Layer_Registry;
    for(i=0;i<Registry->Count;i++){
        layer* l =Registry->Layers[i];
        if(l->OnEvent){
            l->OnEvent(l,e);
            if(e->Handled) break;
        }
    }
}
void destroy_application(void){
    if(!Application) return;
    destroy_layer_registry(Application->Layer_Registry);
    destroy_logging();
    free(Application);
}
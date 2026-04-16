#include "layer.h"
#include <stdlib.h>
#include "debug/log.h"
static inline layer_registry* create_layer_registry(void){
    layer_registry* Registry = (layer_registry*)malloc(sizeof(layer_registry));
    GAVEN_ASSERT(Registry!=NULL,"Couldnt allocate memory to Registry");
    Registry->Layers=NULL;
    Registry->Capacity=0;
    Registry->Count=0;

    Registry->Phase_Buckets=NULL;
    Registry->Phase_Capacity=0;
    Registry->Phase_Count=0;

    return Registry;
}
static inline void destroy_layer_registry(layer_registry* Registry){
    size_t i;
    if(!Registry) return;
    for(i=0;i<Registry->Count;i++) {
        layer* l = Registry->Layers[i];
        if(l) {
            if(l->OnDettach)
                l->OnDettach(l);
            free(l);
        }
    }
    free(Registry->Layers);
    for(i=0;i<Registry->Phase_Count;i++) {
        free(Registry->Phase_Buckets[i].Bindings);
    }
    free(Registry->Phase_Buckets);
    free(Registry);
}
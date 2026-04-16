#include "layer.h"
#include <stdlib.h>
#include "debug/log.h"
#include <string.h>
void push_phase_bucket(phase_bucket* Bucket,layer_binding* Binding){
    if(Bucket->Count>=Bucket->Capacity){
        size_t new_cap = ((Bucket->Capacity == 0)? 4: Bucket->Capacity*2);
        layer_binding** temp = (layer_binding**)realloc(Bucket->Bindings,sizeof(layer_binding*)*new_cap);
        GAVEN_ASSERT(temp,"Failed to allocate memory to phase bucket");
        Bucket->Bindings= temp;
        Bucket->Capacity = new_cap;
    }
    Bucket->Bindings[Bucket->Count++]=Binding;
}
void remove_layer_phase_bucket(phase_bucket* Bucket,layer* Layer){
    size_t i;
    for(i=0;i<Bucket->Count;i++){
        if(Bucket->Bindings[i]->Layer==Layer){
            Bucket->Bindings[i] = Bucket->Bindings[Bucket->Count-1];
            Bucket->Count--;
            break;
        }
        
    }
}
void register_into_phase_bucket(layer_registry* Registry,layer* Layer){
    size_t i,index;
    for(i=0;i<Layer->Bindings_Count;i++){
        layer_binding *b = &Layer->Bindings[i];
        layer_phase phase = b->Phase;
        index=0;
        while(index<Registry->Phase_Count&&Registry->Phase_Buckets[index].Phase<phase){
            index++;
        }
        if  (index<Registry->Phase_Count&&Registry->Phase_Buckets[index].Phase==phase)
            push_phase_bucket(&Registry->Phase_Buckets[index],b);
        else{
            if(Registry->Phase_Count>=Registry->Phase_Capacity){
                size_t new_cap = ((Registry->Phase_Capacity == 0)? 4: Registry->Phase_Capacity*2);
                phase_bucket* temp = (phase_bucket*)realloc(Registry->Phase_Buckets,sizeof(phase_bucket)*new_cap);
                GAVEN_ASSERT(temp,"Failed to allocate memory to registry");
                Registry->Phase_Buckets= temp;
                Registry->Phase_Capacity = new_cap;
            }
            memmove(&Registry->Phase_Buckets[index+1],&Registry->Phase_Buckets[index],(Registry->Phase_Count-index)*sizeof(phase_bucket));
            Registry->Phase_Count++;
            Registry->Phase_Buckets[index].Phase=phase;
            Registry->Phase_Buckets[index].Bindings=NULL;
            Registry->Phase_Buckets[index].Count=0;
            Registry->Phase_Buckets[index].Capacity=0;
            push_phase_bucket(&Registry->Phase_Buckets[index],b);
        }
    }
}


void bind_layer_phase(layer* Layer, layer_phase Phase, void (*Callback)(layer* self,void* phase_ctx),void* Phase_Context){
    if(Layer->Bindings_Count>=Layer->Bindings_Capacity){
        size_t new_cap = ((Layer->Bindings_Capacity == 0)? 4: Layer->Bindings_Capacity*2);
        layer_binding* temp = (layer_binding *)realloc(Layer->Bindings,sizeof(layer_binding)*new_cap);
        GAVEN_ASSERT(temp,"Failed to allocate data to layer bindings");
        Layer->Bindings = temp;
        Layer->Bindings_Capacity= new_cap;
    }
    layer_binding* binding = &Layer->Bindings[Layer->Bindings_Count++];
    binding->Layer=Layer;
    binding->Phase=Phase;
    binding->Phase_Context=Phase_Context;
    binding->Layer_phase_callback=Callback;
}
void add_layer(layer_registry* Layer_Registry, layer* Layer){
    if(Layer_Registry->Count>=Layer_Registry->Capacity){
        size_t new_cap = ((Layer_Registry->Capacity == 0)? 4: Layer_Registry->Capacity*2);
        layer** temp = (layer**)realloc(Layer_Registry->Layers,sizeof(layer*)*new_cap);
        GAVEN_ASSERT(temp,"Failed to allocate data to layer stack");
        Layer_Registry->Layers= temp;
        Layer_Registry->Capacity = new_cap;
    }
    Layer_Registry->Layers[Layer_Registry->Count++]=Layer;
    register_into_phase_bucket(Layer_Registry,Layer);
    if(Layer->OnAttach) Layer->OnAttach(Layer);
}
void remove_layer(layer_registry* Layer_Registry, layer* Layer){
    size_t i,j;
    for (i=0;i<Layer_Registry->Count;i++){
        if(Layer_Registry->Layers[i]==Layer){
            Layer_Registry->Layers[i]=Layer_Registry->Layers[Layer_Registry->Count-1];
            Layer_Registry->Count--;
            if(Layer->OnDettach) Layer->OnDettach(Layer);
            for(j=0;j<Layer_Registry->Phase_Count;j++)
                remove_layer_phase_bucket(&Layer_Registry->Phase_Buckets[j],Layer);
            break;
        }
    }
}
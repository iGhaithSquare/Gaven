#ifndef LAYER_H
#define LAYER_H
#include <stdint.h>
#include <stddef.h>
#include "../src/core/event.h"
typedef struct layer layer;
typedef uint32_t layer_phase;
typedef struct layer_binding{
    layer* Layer;
    layer_phase Phase;
    void* Phase_Context;
    void (*Layer_phase_callback)(layer* self,void* phase_ctx);
}layer_binding;
typedef struct phase_bucket{
    layer_binding** Bindings;
    layer_phase Phase;
    size_t Count;
    size_t Capacity;
}phase_bucket;
typedef struct layer_registry{
    layer** Layers;
    size_t Count;
    size_t Capacity;
    phase_bucket* Phase_Buckets;
    size_t Phase_Count;
    size_t Phase_Capacity;
} layer_registry;
#define create_layer_phase(phase_name,order)\
    enum { layer_phase_##phase_name = (layer_phase)(order) }

struct layer{
    const char* Name;
    layer_binding* Bindings;
    size_t Bindings_Count;
    size_t Bindings_Capacity;
    void (*OnAttach)(layer* self);
    void (*OnDettach)(layer* self);
    void (*OnEvent)(layer* self, event* Event);
    void* LayerData;
};
void add_layer(layer_registry* Layer_Registry, layer* Layer);
void bind_layer_phase(layer* Layer, layer_phase Phase, void (*Callback)(layer* self,void* phase_ctx), void* Phase_Context);
void remove_layer(layer_registry* Layer_Registry, layer* Layer);
#endif
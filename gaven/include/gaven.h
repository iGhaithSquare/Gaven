#ifndef GAVEN_H
#define GAVEN_H
/* LOG_H */
#ifndef LOG_H
#define LOG_H
#include <stdarg.h>
#include <stdint.h>

#ifndef GAVEN_API
#ifdef _WIN32
    #ifdef GAVEN_BUILD_DLL
        #define GAVEN_API __declspec(dllexport)
    #else
        #define GAVEN_API __declspec(dllimport)
    #endif
#else
    #define GAVEN_API __attribute__((visibility("default")))
#endif
#endif
typedef enum{
    GAVEN_COLOR_RESET,
    GAVEN_RED,
    GAVEN_GREEN,
    GAVEN_BLUE,
    GAVEN_YELLOW,
    GAVEN_MAGENTA,
    GAVEN_CYAN,
    GAVEN_WHITE
} GAVEN_COLOR;
#ifdef _WIN32
    #define GAVEN_DEBUG_BREAK() __debugbreak()
#elif defined(__linux__)
    #include <signal.h>
    #define GAVEN_DEBUG_BREAK() raise(SIGTRAP)
#endif
GAVEN_API void GAVEN_PRINT_COLOR(GAVEN_COLOR color, const char* message, ...);
GAVEN_API void GAVEN_PRINT_COLOR_V(GAVEN_COLOR color, const char* message, va_list args);
GAVEN_API void GAVEN_INFO(const char* message, ...);
GAVEN_API void GAVEN_WARN(const char* message, ...);
GAVEN_API void gaven_assert_message(const char* message, const char* file, const char* function, uint32_t line,...);
GAVEN_API void destroy_logging(void);
#ifndef NDEBUG
#define GAVEN_ASSERT(condition, message, ...)\
    do {\
        if (!(condition)) {\
            gaven_assert_message(message, __FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
            GAVEN_DEBUG_BREAK();\
        }\
    }\
    while(0)
#else
#define GAVEN_ASSERT(condition, message, ...)\
    do {\
        if (!(condition)) {\
            gaven_assert_message(message, __FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
        }\
    }\
    while(0)

#endif
#endif
/*LAYER_H*/
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
    void (*Layer_phase_callback)(layer* self,void* phase_ctx);
}layer_binding;
typedef struct phase_bucket{
    layer_binding** Bindings;
    layer_phase Phase;
    size_t Count;
    size_t Capacity;
    void* Phase_Context;
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
GAVEN_API void add_layer(layer_registry* Layer_Registry, layer* Layer);
GAVEN_API void bind_layer_phase(layer* Layer, layer_phase Phase, void (*Callback)(layer* self,void* phase_ctx));
GAVEN_API void bind_phase_ctx(layer_registry* Registry, layer_phase Phase, void* Phase_Context);
GAVEN_API void remove_layer(layer_registry* Layer_Registry, layer* Layer);
#endif
/*HTTP_H*/
#include "../src/networking/http.h"
/* Application.h */
typedef struct application application;

struct application{
    uint8_t Running;
    layer_registry* Layer_Registry;
};

GAVEN_API void application_event_callback(event *e);
GAVEN_API application* create_gaven_application(void);
GAVEN_API void run_application(void);
GAVEN_API void destroy_application(void);
/* Macros */
create_layer_phase(polling,0);
#endif
#ifndef GAVEN_H
#define GAVEN_H
/* Headers */
#include "../src/core/debug/log.h"
#include "../src/core/layer/layer.h"
#include "../src/networking/http.h"
/* Application.h */
#include <stdint.h>
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
#ifndef GAVEN_H
#define GAVEN_H

/* Headers */
#include "../src/core/debug/log.h"
#include "../src/core/event.h"
#include "../src/core/layer/layer.h"
#include "../src/networking/http.h"
/* Application.h */
#include <stdint.h>
typedef struct application application;

struct application{
    uint8_t Running;
    layer_registry* Layer_Registry;
};

void application_event_callback(event *e);
application* create_gaven_application(void);
#ifdef NO_GAVEN_MAIN
void run_application(void);
void destroy_application(void);
#endif
/* Macros */
create_layer_phase(polling,0);
#ifndef GAVEN_MAIN
#endif
#endif
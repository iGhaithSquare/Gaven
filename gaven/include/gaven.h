#ifndef GAVEN_H
#define GAVEN_H

/* Application.h */
#include <stdint.h>
typedef struct application application;

struct application{
    uint8_t running;
};
application* create_gaven_application();
#ifdef NO_GAVEN_MAIN
void run_application(application* self);
void destroy_application(application* self);
#endif

/* Next header public implementation*/

#ifndef GAVEN_MAIN
#endif
#endif
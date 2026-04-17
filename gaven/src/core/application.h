#ifndef APPLICATION_H
#define APPLICATION_H
#include "gaven.h"
#ifndef NO_GAVEN_MAIN
void run_application(application* self);
void destroy_application(application* self);
#endif
#endif
#define GAVEN_INTERNAL
#include "application.h"
#ifndef NO_GAVEN_MAIN
extern application* gaven_main(int argc, char** argv);
int main(int argc, char** argv){
    application* mainApp = gaven_main(argc,argv);
    run_application(mainApp);
    destroy_application(mainApp);
    return 0;
}
#endif
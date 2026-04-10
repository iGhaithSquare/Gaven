
#include "gaven.h"

/* The user can either use the prebuilt main or create his own*/
/* to create our own main we first define NO_GAVEN_MAIN */
/* Using our own main*/
#ifdef NO_GAVEN_MAIN
int main(){
    /* We create the application*/
    application* app = create_gaven_application();
    /* We set the running to true*/
    app->running=1;
    /* User Specific Code */
    GAVEN_WARN("HELLO WORLD");
    /* We call the run loop */
    run_application(app);
    /* We destroy the application*/
    destroy_application(app);
    return 0;
}
#else
/* To use the prebuilt main we dont define anything */
/* Using the prebuilt Gaven Main workflow */
application* gaven_main(int argc, char** argv){
    /* We create the application*/
    application* app = create_gaven_application();
    /* User Specific Code */
    GAVEN_WARN("HELLO WORLD");
    GAVEN_INFO("ANOTHER THING TO BE PRINTED");
    /* We return the application*/
    return app;
}
#endif
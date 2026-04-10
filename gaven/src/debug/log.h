#ifndef LOG_H
#define LOG_H
#include <stdarg.h>
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
void GAVEN_PRINT_COLOR(GAVEN_COLOR color, const char* message, ...);
void GAVEN_INFO(const char* message, ...);
void GAVEN_WARN(const char* message, ...);
void GAVEN_ASSERT(int condition, const char* message);

#endif
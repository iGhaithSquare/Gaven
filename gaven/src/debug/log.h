#ifndef LOG_H
#define LOG_H
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
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
void GAVEN_PRINT_COLOR(GAVEN_COLOR color, const char* message, ...);
void GAVEN_PRINT_COLOR_V(GAVEN_COLOR color, const char* message, va_list args);
void GAVEN_INFO(const char* message, ...);
void GAVEN_WARN(const char* message, ...);
void gaven_assert_message(const char* message, const char* file, const char* function, uint32_t line,...);
#ifdef GAVEN_DEBUG
#define GAVEN_ASSERT(condition, message, ...)\
    do {\
        if (!(condition)) {\
            gaven_assert_message(message, __FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);\
            GAVEN_DEBUG_BREAK();\
        }\
    }\
    while(0)
#else
#define GAVEN_ASSERT(condition, message, ...) ((void)0)

#endif
#endif
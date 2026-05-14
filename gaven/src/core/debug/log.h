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
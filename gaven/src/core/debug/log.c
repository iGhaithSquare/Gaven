#include "log.h"
#include <stdio.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
WORD GAVEN_COLOR_TRANSLATE(GAVEN_COLOR color){
    switch (color){
        case GAVEN_RED:       return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case GAVEN_GREEN:     return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case GAVEN_BLUE:      return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case GAVEN_YELLOW:    return FOREGROUND_RED | FOREGROUND_GREEN;
        case GAVEN_MAGENTA:   return FOREGROUND_RED | FOREGROUND_BLUE;
        case GAVEN_CYAN:      return FOREGROUND_GREEN | FOREGROUND_BLUE;
        default:        GAVEN_WARN("COLOR NOT SUPPORTED"); /* no break for fallthrough*/
        case GAVEN_WHITE:     return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; /* default fallback */
    }
}
uint8_t check_folder_exists(const char* path){
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES)&&(attrs&FILE_ATTRIBUTE_DIRECTORY);
}
void create_folder(const char* path){
    CreateDirectoryA(path,NULL);
    GAVEN_ASSERT(check_folder_exists(path),"COULD NOT CREATE LOG FOLDER");
}
#elif defined(__linux__)
#include <sys/stat.h>
const char* GAVEN_COLOR_TRANSLATE(GAVEN_COLOR color){
    switch (color){
        case GAVEN_RED:       return "\033[31m";
        case GAVEN_GREEN:     return "\033[32m";
        case GAVEN_BLUE:      return "\033[34m";
        case GAVEN_YELLOW:    return "\033[33m";
        case GAVEN_MAGENTA:   return "\033[35m";
        case GAVEN_CYAN:      return "\033[36m";
        case GAVEN_WHITE:     return "\033[37m";
        default:        GAVEN_WARN("COLOR NOT SUPPORTED"); /* no break for fallthrough*/
        case GAVEN_COLOR_RESET:     return "\033[0m"; /* default fallback */
    }
}
uint8_t check_folder_exists(const char* path){
    struct stat info;
    return (stat(path,&info)==0)&&(info.st_mode&S_IFDIR);
}
void create_folder(const char* path){
    mkdir(path,0755);
    GAVEN_ASSERT(check_folder_exists(path),"COULD NOT CREATE LOG FOLDER");
}
#else
void char* GAVEN_COLOR_TRANSLATE(GAVEN_COLOR color){
    GAVEN_ASSERT(0,"Platform Not Supported");
}
#endif
void GAVEN_PRINT_COLOR_V(GAVEN_COLOR color, const char* message, va_list args){
    #ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole,&csbi);
    WORD originalColor = csbi.wAttributes;
    SetConsoleTextAttribute(hConsole,GAVEN_COLOR_TRANSLATE(color));
    vprintf(message,args);
    SetConsoleTextAttribute(hConsole,originalColor);
    #elif defined(__linux__)
    printf("%s",GAVEN_COLOR_TRANSLATE(color));
    vprintf(message,args);
    printf("%s",GAVEN_COLOR_TRANSLATE(GAVEN_COLOR_RESET));
    #else
    GAVEN_ASSERT(0,"Platform Not Supported");
    #endif
}
void GAVEN_PRINT_COLOR(GAVEN_COLOR color, const char* message, ...){
    va_list args;
    va_start(args, message);
    GAVEN_PRINT_COLOR_V(color,message,args);
    va_end(args);
}
char* gaven_get_time(const char* format, char* buffer, size_t size){
    time_t rawtime;
    struct tm timeinfo;
    time(&rawtime);
    #ifdef _WIN32
        localtime_s(&timeinfo,&rawtime);
    #elif  defined(__linux__)
        localtime_r(&rawtime,&timeinfo);
    #endif
    strftime(buffer,size,format,&timeinfo);
    return buffer;
}
#ifdef GAVEN_DEBUG
static FILE* get_logFile(void){
    char buffer[64];
    static FILE* log = NULL;
    if (log!=NULL)
        return log;
    if(!check_folder_exists("logs")) create_folder("logs");
    gaven_get_time("logs/log_%Y-%m-%d_%H-%M-%S.txt",buffer,sizeof(buffer));
    log = fopen(buffer,"w");
    GAVEN_ASSERT(log!=NULL,"COULD NOT CREATE LOG FILE");
    return log;
}
void log_to_file(const char* message) {
    FILE *log = get_logFile();
    fprintf(log,"%s",message);
    fflush(log);
}
void GAVEN_WARN(const char* message, ...){
    va_list args;
    char time_buffer[16];
    char msg_buffer[512];
    char final_buffer[528];
    gaven_get_time("%H:%M:%S",time_buffer,sizeof(time_buffer));
    va_start(args,message);
    vsnprintf(msg_buffer,sizeof(msg_buffer),message,args);
    va_end(args);
    snprintf(final_buffer,sizeof(final_buffer),"[%s] WARNING: %s\n",time_buffer,msg_buffer);
    GAVEN_PRINT_COLOR(GAVEN_RED,final_buffer);
    log_to_file(final_buffer);
}
void GAVEN_INFO(const char* message, ...){
    va_list args;
    char time_buffer[16];
    char msg_buffer[512];
    char final_buffer[528];
    gaven_get_time("%H:%M:%S",time_buffer,sizeof(time_buffer));
    va_start(args,message);
    vsnprintf(msg_buffer,sizeof(msg_buffer),message,args);
    va_end(args);
    snprintf(final_buffer,sizeof(final_buffer),"[%s] INFO: %s\n",time_buffer,msg_buffer);
    GAVEN_PRINT_COLOR(GAVEN_GREEN,final_buffer);
    log_to_file(final_buffer);
}
void gaven_assert_message(const char* message, const char* file, const char* function, uint32_t line,...){
    va_list args;
    char time_buffer[16];
    char msg_buffer[512];
    char final_buffer[528];
    gaven_get_time("%H:%M:%S",time_buffer,sizeof(time_buffer));
    va_start(args,line);
    vsnprintf(msg_buffer,sizeof(msg_buffer),message,args);
    va_end(args);
    snprintf(final_buffer,sizeof(final_buffer),"[%s] ASSERT: %s\nFILE: %s FUNCTION: %s LINE: %d",time_buffer,msg_buffer,file,function,line);
    GAVEN_PRINT_COLOR(GAVEN_RED,final_buffer);
    log_to_file(final_buffer);
}

void destroy_logging(void){
    fclose(get_logFile());
}
#else
void GAVEN_WARN(const char* message, ...){ return;} 
void GAVEN_INFO(const char* message, ...){ return;}
void gaven_assert_message(const char* message, const char* file, const char* function, uint32_t line,...){ return;}
void destroy_logging(void){ return; }
#endif
#ifndef EVENT_H
#define EVENT_H
#include "debug/log.h"
#ifdef EVENT_CATEGORY_64
typedef uint64_t event_category;
#define EVENT_CATEGORY_SIZE 64
#else
typedef uint32_t event_category;
#define EVENT_CATEGORY_SIZE 32
#endif
#define EVENT_CATEGORY_BIT(x) ((event_category)1 <<  (x))
/* creates an event category with the name "name".
Note: bit needs to be unique number from 0 to 31 or to 63 if EVENT_CATEGORY_64 is defined*/
#define create_event_category(name,bit)\
    enum { event_category_##name = (event_category)(EVENT_CATEGORY_BIT(bit)) };\
    typedef char event_category_##name##_bit_check[((bit)>=0&&(bit)<EVENT_CATEGORY_SIZE)? 1:-1]; /* doesnt compile if bit is not a constant that is <0 or >EVENT_CATEGORY_SIZE*/
typedef uint32_t event_type;
/* creates an event type, it is very important that you write the name of the type without ""
Note: value needs to be a unique positive integer*/
#define create_event_type(struct_event_type,value)\
    enum { event_type_##struct_event_type = (event_type)(value) }

typedef struct event{
    event_type Type;
    event_category Category_Flags;
    uint8_t Handled;
    const char* Name;
    void (*To_String)(struct event *Event, char* buffer, size_t buffer_size);
}event;
static inline uint8_t is_event_in_category(event *Event,event_category Category){ return (Event->Category_Flags & Category)!=0;}
#define event_dispatch(Event_Pointer, Event_Type, Dispatch_Function)\
    do{\
        if((Event_Pointer)!=NULL && (Event_Pointer)->Type == event_type_##Event_Type)\
            (Event_Pointer)->Handled = Dispatch_Function((Event_Type*)(Event_Pointer));\
    } while(0)
#endif
#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#ifdef GAVEN_DYNAMIC_ARRAY_INT
typedef struct dynamic_array_int_header{
    int Count;
    int Cap;
} dynamic_array_int_header;

#define DYNAMIC_ARRAY_INT_CREATE(array,capacity)\
    do{\
        dynamic_array_int_header* daih=NULL;\
        size_t cap=(capacity)?(capacity):128;\
        daih=malloc(sizeof(dynamic_array_int_header)+sizeof(*array)*cap);\
        daih->Count=0;\
        daih->Cap=cap;\
        array=(void*)(daih+1);\
    }while(0)
#define DYNAMIC_ARRAY_INT_PUSH(array,item)\
    do{\
        dynamic_array_int_header* daih=(dynamic_array_int_header*)(array)-1;\
        if(daih->Count>=daih->Cap){\
            daih->Cap*=2;\
            daih=realloc(daih,sizeof(dynamic_array_int_header)+sizeof(*array)*(daih->Cap));\
            array=(void*)(daih+1);\
        }\
        array[daih->Count++]=(item);\
    }while(0)
#define DYNAMIC_ARRAY_INT_SWAP_REMOVE(array,item)\
    do{\
        dynamic_array_int_header* daih=(dynamic_array_int_header*)(array)-1;\
        if((item)>=array&&(item)<array+daih->Count)\
            *(item)=array[--daih->Count];\
    }while(0)


#define DYNAMIC_ARRAY_INT_FREE(array)\
    do{\
        if(array){\
            free((dynamic_array_int_header*)(array)-1);\
            (array)=NULL;\
        }\
    }while(0)
#endif


#ifdef GAVEN_DYNAMIC_ARRAY_SIZET
typedef struct dynamic_array_sizet_header{
    size_t Count;
    size_t Cap;
} dynamic_array_sizet_header;

#define DYNAMIC_ARRAY_SIZET_CREATE(array,capacity)\
    do{\
        dynamic_array_sizet_header* daih=NULL;\
        size_t cap=(capacity)?(capacity):128;\
        daih=malloc(sizeof(dynamic_array_sizet_header)+sizeof(*array)*cap);\
        daih->Count=0;\
        daih->Cap=cap;\
        array=(void*)(daih+1);\
    }while(0)
#define DYNAMIC_ARRAY_SIZET_PUSH(array,item)\
    do{\
        dynamic_array_sizet_header* daih=(dynamic_array_sizet_header*)(array)-1;\
        if(daih->Count>=daih->Cap){\
            daih->Cap*=2;\
            daih=realloc(daih,sizeof(dynamic_array_sizet_header)+sizeof(*array)*(daih->Cap));\
            array=(void*)(daih+1);\
        }\
        array[daih->Count++]=(item);\
    }while(0)
#define DYNAMIC_ARRAY_SIZET_SWAP_REMOVE(array,item)\
    do{\
        dynamic_array_sizet_header* daih=(dynamic_array_sizet_header*)(array)-1;\
        if((item)>=array&&(item)<array+daih->Count)\
            *(item)=array[--daih->Count];\
    }while(0)
    
#define DYNAMIC_ARRAY_SIZET_FREE(array)\
    do{\
        if(array){\
            free((dynamic_array_sizet_header*)(array)-1);\
            (array)=NULL;\
        }\
    }while(0)
#endif

#endif
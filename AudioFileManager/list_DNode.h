#ifndef __LIST_DNODE_H__
#define __LIST_DNODE_H__

#include <stdint.h>
typedef void (*Destructor)(void *);
typedef struct list_DNode list_DNode;

struct list_DNode
{
    list_DNode *prev;
    list_DNode *next;
    union
    {
        int sz;
        struct
        {
            uint32_t typesz : 24;
            uint32_t typeID : 7;
            uint32_t is_multiType : 1;
        } type;
    } datainf;
    uint8_t data[];
};

list_DNode *Node_alloc(int8_t type, int sz);

list_DNode* Node_free(list_DNode *f, Destructor* destructors);

list_DNode* Node_prev(list_DNode* cur,list_DNode* prevs);

void Node_link(list_DNode* current, list_DNode* prev, list_DNode* next);


#define Node_getData(Type,node) (*(Type*)(node)->data)
#define Node_getArray(Type,node) ((Type*)(node)->data)

#endif
#ifndef __NODE_H__
#define __NODE_H__
#include <stdint.h>

typedef void (*Destructor)(void *);
typedef struct Node Node;

struct Node
{
    Node *next;
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

Node *Node_alloc(int8_t type, int sz);

Node* Node_free(Node *f, Destructor* destructors);

#define Node_getData(Type,node) (*(Type*)(node)->data)
#define Node_getArray(Type,node) ((Type*)(node)->data)

#endif
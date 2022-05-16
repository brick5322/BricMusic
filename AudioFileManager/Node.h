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
        uint32_t sz;
        struct
        {
            uint32_t typesz : 24;
            uint32_t typeID : 8;
        } type;
    } datainf;
    uint8_t data[];
};

Node *Node_alloc(int type, int sz);

Node* Node_free(Node *f, Destructor* destructors);

#define Node_getdata(node) ((node)->data)

#endif
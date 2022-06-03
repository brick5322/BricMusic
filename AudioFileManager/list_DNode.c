#include "list_DNode.h"
#include <stdlib.h>

list_DNode *Node_alloc(int8_t type, int sz)
{
    if (sz < 0 || type < 0)
        return NULL;
    list_DNode *ret = malloc(sizeof(list_DNode) + sz);
    if (!ret)
        return ret;
    ret->datainf.sz = sz;
    if (type)
    {
        ret->datainf.type.is_multiType = 1;
        ret->datainf.type.typeID = type;
    }
    ret->prev = NULL;
    ret->next = NULL;
    return ret;
}

list_DNode *Node_free(list_DNode *f, Destructor *destructors)
{
    list_DNode *next = f->next;
    Destructor destructor = f->datainf.type.is_multiType? destructors[f->datainf.type.typeID]:destructors[0];
    if (destructor)
        destructor(f->data);
    free(f);
    return next;
}

void Node_link(list_DNode* current,list_DNode *prev,list_DNode *next)
{
    prev->next = current;
    current->prev = prev;
    current->next = next;
    next->prev = current;
}

list_DNode* Node_prev(list_DNode* cur,list_DNode* prevs)
{
    return cur->prev;
}

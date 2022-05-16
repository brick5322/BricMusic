#include "Node.h"
#include <stdlib.h>

Node *Node_alloc(int type, int sz)
{
    if (type && sz > 0x00ffffff)
        return NULL;
    Node *ret = malloc(sizeof(Node) + sz);
    if (!ret)
        return ret;
    ret->datainf.sz = sz;
    ret->datainf.type.typeID = type;
    ret->next = NULL;
    return ret;
}

Node *Node_free(Node *f, Destructor *destructors)
{
    Node *next = f->next;
    if (f->datainf.type.typeID)
        destructors[f->datainf.type.typeID](f->data);
    free(f);
    return next;
}

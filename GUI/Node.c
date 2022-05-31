#include "Node.h"
#include <stdlib.h>

Node *Node_alloc(int8_t type, int sz)
{
    if (sz < 0 || type < 0)
        return NULL;
    Node *ret = malloc(sizeof(Node) + sz);
    if (!ret)
        return ret;
    ret->datainf.sz = sz;
    if (type)
    {
        ret->datainf.type.is_multiType = 1;
        ret->datainf.type.typeID = type;
    }
    ret->next = NULL;
    return ret;
}

Node *Node_free(Node *f, Destructor *destructors)
{
    Node *next = f->next;
    Destructor destructor = f->datainf.type.is_multiType? destructors[f->datainf.type.typeID]:destructors[0];
    if (destructor)
        destructor(f->data);
    free(f);
    return next;
}

int Node_size(Node* node)
{
    return node->datainf.sz < 0 ? node->datainf.type.typesz : node->datainf.sz;
}

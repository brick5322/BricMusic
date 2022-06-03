#ifndef __LOOPLIST_H__
#define __LOOPLIST_H__

#include <stdint.h>
typedef void (*Destructor)(void *);
typedef struct list_DNode Node;


typedef struct LoopList
{
    Node* first;
    Node* latest;

    uint32_t latestPos;
    uint32_t length;

    uint8_t nb_types;
    Destructor destructors[];
}LoopList;

LoopList* LoopList_alloc(int nb_types);

void LoopList_free(LoopList *lst);

void LoopList_clear(LoopList *lst);

Node *LoopList_add(LoopList *lst, int type, uint32_t len);

Node* LoopList_get(LoopList *lst, int pos_at);

void LoopList_set_Destructor(LoopList* lst, int nb_type, Destructor destructor);

#endif
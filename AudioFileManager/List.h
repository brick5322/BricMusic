#ifndef __LIST_H__
#define __LIST_H__

#include <stdint.h>

typedef struct Node Node;
typedef void (*Destructor)(void*);

typedef struct BasicList
{
    Node* first;
    Node* last;
    Node* latest;

    uint32_t latestPos;
    uint32_t length;

    uint8_t nb_types;
    Destructor destructors[];
}BasicList;

BasicList* BasicList_alloc(int nb_types);

void BasicList_free(BasicList *lst);

void BasicList_clear(BasicList *lst);

Node *BasicList_add(BasicList *lst, int type, uint32_t len);

Node *BasicList_insert(BasicList *lst, uint32_t pos_at, int type, uint32_t len);

void BasicList_delete(BasicList *lst,uint32_t pos_at);

Node* BasicList_update(BasicList *lst,uint32_t pos_at,int type, uint32_t len);

Node* BasicList_get(BasicList *lst,uint32_t pos_at);

void BasicList_set_Destructor(BasicList* lst, int nb_type, Destructor destructor);

#endif
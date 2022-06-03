#include "LoopList.h"
#include "list_DNode.h"
#include <stdlib.h>

LoopList *LoopList_alloc(int nb_types)
{
    LoopList *ret = malloc(sizeof(LoopList) + sizeof(Destructor) * (nb_types + 1));
    if (!ret)
        return ret;
    ret->first = NULL;
    ret->latest = NULL;
    ret->length = 0;
    ret->nb_types = nb_types;
    ret->latestPos = 0;
    for (int i = 0; i <= nb_types; i++)
        ret->destructors[i] = NULL;
    return ret;
}

void LoopList_free(LoopList *lst)
{
    LoopList_clear(lst);
    free(lst);
}

void LoopList_clear(LoopList *lst)
{
    Node *node = lst->first;
    while (node != NULL)
        node = Node_free(node, lst->destructors);
    lst->first = NULL;
    lst->latest = NULL;
    lst->length = 0;
    lst->latestPos = 0;
}

Node *LoopList_add(LoopList *lst, int type, uint32_t len)
{
    Node *ret = Node_alloc(type, len);
    if (!ret)
        return ret;
    if (!lst->length)
    {
        lst->first = ret;
        lst->latest = ret;
        lst->latestPos = 0;
        Node_link(ret, ret, ret);
    }
    else
    {
        Node_link(ret, lst->latest, lst->latest->next);
        lst->latest = ret;
        lst->latestPos++;
    }
    lst->length++;
    return ret;
}

Node* LoopList_get(LoopList* lst, int pos_at)
{
    if (lst->first == NULL)
        return NULL;
	lst->latest = lst->first;
    if (pos_at >= 0)
    {
        lst->latestPos = pos_at % lst->length;
        for (int i = 0; i < pos_at; i++)
            lst->latest = lst->latest->next;
    }
    else
    {
        lst->latestPos = lst-> length+(pos_at % lst->length);
        for (int i = 0; i < -pos_at; i++)
            lst->latest = lst->latest->prev;
    }
	return lst->latest;
}

//Node *LoopList_insert(LoopList *lst, uint32_t pos_at, int type, uint32_t len)
//{
//
//}
//
//void LoopList_delete(LoopList *lst, uint32_t pos_at);
//
//Node *LoopList_update(LoopList *lst, uint32_t pos_at, int type, uint32_t len);

void LoopList_set_Destructor(LoopList *lst, int nb_type, Destructor destructor)
{
    if (nb_type <= lst->nb_types)
        lst->destructors[nb_type] = destructor;
}
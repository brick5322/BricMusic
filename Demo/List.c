#include "List.h"
#include "Node.h"
#include <stdlib.h>
#include <string.h>

BasicList *BasicList_alloc(int nb_types)
{
    BasicList *ret = malloc(sizeof(BasicList) + sizeof(Destructor) * (nb_types + 1));
    if (!ret)
        return ret;
    ret->first = NULL;
    ret->last = NULL;
    ret->latest = NULL;
    ret->length = 0;
    ret->nb_types = nb_types;
    ret->latestPos = -1;
    for(int i = 0;i <=nb_types;i++)
        ret->destructors[i] = NULL;
    return ret;
}

void BasicList_free(BasicList *lst)
{
    BasicList_clear(lst);
    free(lst);
}

void BasicList_clear(BasicList *lst)
{
    Node *node = lst->first;
    while (node != NULL)
        node = Node_free(node, lst->destructors);
    lst->first = NULL;
    lst->last = NULL;
    lst->latest = NULL;
    lst->length = 0;
    lst->latestPos = -1;

}

Node *BasicList_add(BasicList *lst, int type, uint32_t len)
{
    Node *ret = Node_alloc(type, len);
    if (!ret)
        return ret;

    if (lst->last)
    {
        lst->last->next = ret;
        lst->last = lst->last->next;
    }
    else
    {
        lst->last = ret;
        lst->first = lst->last;
    }
    lst->latest = ret;
    lst->length++;
    lst->latestPos = lst->length - 1;

    return ret;
}

Node *BasicList_insert(BasicList *lst, uint32_t pos_at, int type, uint32_t len)
{
    Node *ret = Node_alloc(type, len);
    Node *tmp = NULL;
    if (!ret)
        return ret;
    if (!pos_at)
    {
        ret->next = lst->first;
        lst->first = ret;
        lst->latest = ret;
        lst->latestPos = 0;
        if (!lst->last)
            lst->last = ret;
    }
    else
    {
        Node *tmp = BasicList_get(lst, pos_at - 1);
        if (!tmp)
            goto nullret;
        if (tmp == lst->last)
            lst->last = ret;
        ret->next = tmp->next;
        tmp->next = ret;
        lst->latest = ret;
        lst->latestPos++;
    }
    lst->length++;
    return ret;
nullret:
    Node_free(ret, lst->destructors);
    return NULL;
}

Node* BasicList_remove(BasicList* lst, uint32_t pos_at)
{
    Node* ret = NULL;
    Node* tmp = NULL;
    if (!pos_at)
    {
        ret = lst->first;
        lst->first = ret->next;
        lst->length--;
        return ret;
    }
    tmp = BasicList_get(lst, pos_at - 1);
    if (!tmp)
        return tmp;
    ret = tmp->next;
    if (ret == lst->last)
        lst->last = tmp;
    if (!ret)
        return ret;
    tmp->next = ret->next;
    lst->length--;
    return ret;
}

void BasicList_delete(BasicList *lst, uint32_t pos_at)
{
    Node* del = BasicList_remove(lst, pos_at);
    if (del)
        Node_free(del, lst->destructors);
}

Node* BasicList_update(BasicList* lst, uint32_t pos_at, int type, uint32_t len)
{
    Node* tmp_before = BasicList_get(lst, pos_at - 1);
    if (!tmp_before)
        return NULL;
    Node* tmp_replaced = tmp_before->next;
    if (!tmp_replaced)
        return NULL;
    Node* ret = Node_alloc(type, len);
    ret->next = tmp_replaced->next;
    tmp_before->next = ret;
    Node_free(tmp_replaced, lst->destructors);
    return ret;
}

Node *BasicList_get(BasicList *lst, uint32_t pos_at)
{
    Node *ret = NULL;
    if (pos_at >= lst->length)
        return ret;
    if (!lst->length)
        return ret;
    if (pos_at == lst->length)
    {
        ret = lst->last;
        lst->latest = ret;
        lst->latestPos = lst->length;
        return ret;
    }
    if (pos_at < lst->latestPos)
    {
        lst->latestPos = 0;
        lst->latest = lst->first;
    }
    for (; lst->latestPos < pos_at; lst->latestPos++)
        lst->latest = lst->latest->next;
    return lst->latest;
}

void BasicList_set_Destructor(BasicList* lst, int nb_type, Destructor destructor)
{
    if (nb_type <= lst->nb_types)
        lst->destructors[nb_type] = destructor;
}

Node* BasicList_queue_in(BasicList* lst, int type, uint32_t len)
{
    return BasicList_insert(lst, 0, type, len);
}

Node* BasicList_queue_out(BasicList* lst)
{
    return BasicList_remove(lst, lst->length-1);
}

Node* BasicList_stack_push(BasicList* lst, int type, uint32_t len)
{
    return BasicList_insert(lst, 0, type, len);
}

Node* BasicList_stack_pop(BasicList* lst)
{
    return BasicList_remove(lst,0);
}

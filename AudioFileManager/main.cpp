#include <QtCore/QCoreApplication>
#include <iostream>

extern"C"
{
#include "List.h"
#include "Node.h"
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BasicList* lst = BasicList_alloc(0);
    int p = 0;
    for (int i = 0; i < 10; i++)
        *(int*)(BasicList_add(lst, 0, sizeof(int))->data) = i;
    for (int i = 0; i < lst->length; i++)
        std::cout << i << ":" << *(int*)BasicList_get(lst, i)->data << std::endl;
    //BasicList_delete(lst, 8);
    //*(int*)BasicList_insert(lst, 8, 0, sizeof(int))->data = 114514;
    *(int*)BasicList_update(lst, 8, 0, sizeof(int))->data = 114514;
    for (int i = 0; i < lst->length; i++)
        std::cout << i << ":" << *(int*)BasicList_get(lst, i)->data << std::endl;
    return 0;
    return a.exec();
}

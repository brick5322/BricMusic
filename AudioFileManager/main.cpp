#include <QtCore/QCoreApplication>
#include <iostream>
#include "List_cstr.h"

extern"C"
{
#include "List.h"
#include "Node.h"
}

void test0()
{
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
}

void test1()
{
    const char* teststr = "hello world!\n";
    char testarr[] = "hello World!\n";
    List_cstr list_str;
    list_str.add(teststr);
    list_str.add(teststr, move);
    list_str.add(testarr);
    list_str.add(testarr, move);
    testarr[5] = '-';
    for (int i = 0; i < 4; i++)
        std::cout << list_str[i] << std::endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    test1();
    return 0;
    return a.exec();
}

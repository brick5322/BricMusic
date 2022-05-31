#include "List_cstr.h"
#include <cstring>

extern"C"
{
#include "List.h"
#include "Node.h"
}

List_cstr::List_cstr(Flag flag):basic_list(BasicList_alloc(2)),flag(flag)
{
	if (!basic_list)
		throw 1;
}

List_cstr::~List_cstr()
{
	BasicList_free(basic_list);
}

bool List_cstr::add(const char* src)
{
	size_t len = strlen(src) + 1;
	Node* add = nullptr;
	switch (flag)
	{
	case copy:
		add = BasicList_add(basic_list, copy, len);
		if (!add)
			return true;
		strcpy(Node_getArray(char,add), src);
		break;
	case move:
		add = BasicList_add(basic_list, move, sizeof(char*));
		if (!add)
			return true;
		Node_getData(const char*, add) = src;
		break;
	default:
		break;
	}
	return false;
}

bool List_cstr::insert(const char* src, uint32_t pos)
{
	size_t len = strlen(src) + 1;
	Node* ins = nullptr;
	switch (flag)
	{
	case copy:
		ins = BasicList_insert(basic_list,pos, copy, len);
		if (!ins)
			return true;
		strcpy(Node_getArray(char, ins), src);
		break;
	case move:
		ins = BasicList_insert(basic_list,pos, move, sizeof(char*));
		if (!ins)
			return true;
		Node_getData(const char*, ins) = src;
		break;
	default:
		break;
	}
	return false;
}

void List_cstr::remove(uint32_t pos)
{
	BasicList_delete(basic_list, pos);
}

const char* List_cstr::operator[](uint32_t pos)
{
	Node* current = BasicList_get(basic_list,pos);
	if (!current)
		return nullptr;
	switch (current->datainf.type.typeID)
	{
	case copy:
		return Node_getArray(char, current);
	case move:
		return Node_getData(char*,current);
	default:
		break;
	}
}

bool List_cstr::update(const char* src, uint32_t pos)
{
	size_t len = strlen(src) + 1;
	Node* upd = nullptr;
	switch (flag)
	{
	case copy:
		upd = BasicList_update(basic_list, pos, copy, len);
		if (!upd)
			return true;
		strcpy(Node_getArray(char, upd), src);
		break;
	case move:
		upd = BasicList_update(basic_list, pos, move, sizeof(char*));
		if (!upd)
			return true;
		Node_getData(const char*, upd) = src;
		break;
	default:
		break;
	}
	return false;
}

size_t List_cstr::length()
{
	return basic_list->length;
}

List_cstr& List_cstr::operator()(Flag flag)
{
	this->flag = flag;
	return *this;
}

List_cstr& List_cstr::operator<<(const char* src)
{
	Node* queue_in = nullptr;
	size_t len = 0;
	switch (flag)
	{
	case copy:
		len = strlen(src) + 1;
		queue_in = BasicList_queue_in(basic_list, copy, len);
		if (!queue_in)
			return *this;
		strcpy(Node_getArray(char, queue_in), src);
		break;
	case move:
		queue_in = BasicList_queue_in(basic_list, move, sizeof(char*));
		if (!queue_in)
			return *this;
		Node_getData(const char*, queue_in) = src;
		break;
	default:
		break;
	}
	return *this;
}

List_cstr& List_cstr::operator>>(const char*& dst)
{
	Node* queue_out = BasicList_queue_out(basic_list);
	if (!queue_out)
		return *this;
	switch (queue_out->datainf.type.typeID)
	{
	case copy:
		dst = new char[queue_out->datainf.type.typesz];
		memcpy((char*)dst, Node_getArray(const char, queue_out), queue_out->datainf.type.typesz);
		break;
	case move:
		dst = Node_getData(const char*, queue_out);
		break;
	default:
		break;
	}
	Node_free(queue_out, basic_list->destructors);
	return *this;
}

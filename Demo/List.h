/**
  ******************************************************************************
  * @file    List.h
  * @author  LIUVBC
  * @version V1.0
  * @date    13-Sep-2021
  ******************************************************************************
**/

#pragma once

enum ErrNum{OverFlow, EmptyList};

template <typename T>
class List;


template <typename T>
class Node
{
typedef void (*Destory)(T data);

public:
	T& get_data(){
		return data;
	}
private:
	friend List<T>;
	Node(T data,Node<T>*next,Destory fun) :Data_Destory(fun),data(data),next(next) {};
	~Node();
	void (*Data_Destory)(T data);
	Node* next;
	T data;
};

template <typename T>
class List
{
	typedef void (*Destory)(T data);

public:
	List(Destory fun = nullptr);
	~List();
	void Add_Data(T data);
	void Insert_Data(int after_where, T data);
	void Delete_Data(unsigned int pos);
	Node<T>& operator[](unsigned int pos);
	bool is_empty();
	int Length() {
		return length;
	}
private:
	void (*Data_Destory)(T data);

	friend Node<T>;

	unsigned int length;
	Node<T>* First;

	Node<T>* Latest;
	unsigned int num_latest;

	Node<T>* Last;
};



template <typename T>
Node<T>::~Node()
{
	if (Data_Destory)
		Data_Destory(this->data);
	if (this->next)
		delete this->next;
}

template <typename T>
List<T>::List(Destory fun):Data_Destory(fun), length(0), First(nullptr), Latest(nullptr), num_latest(-1), Last(nullptr) {};

template <typename T>
List<T>::~List()
{
	if (length)
		delete First;
}


template <typename T>
void List<T>::Add_Data(T data)
{
	if (First)
	{
		Last->next = new Node<T>(data,nullptr,Data_Destory);
		Last = Last->next;
	}
	else
	{
		First = new Node<T>(data,nullptr,Data_Destory);
		Last = First;
	}
	Latest = Last;
	length++;
	num_latest = length;
	return;
}

template <typename T>
void List<T>::Insert_Data(int after_where,T data)
{
	Latest = (*this)[after_where].next = new Node<T>(data, (*this)[after_where].next, Data_Destory);
	num_latest = after_where + 1;
	length++;
}

template <typename T>
void List<T>::Delete_Data(unsigned int pos)
{
	if (pos >= length)
		return;

	int lpos = pos - 1;
	Node<T>* Npos = nullptr;

	if (pos >= num_latest)
	{
		Npos = Latest;
		pos -= num_latest;
		for (unsigned int i = 1;i < pos;i++)
			Npos = Npos->next;
	}
	else
	{
		Npos = First;
		for (unsigned int i = 1; i < pos;i++)
			Npos = Npos->next;
	}
	Latest = Npos;
	Npos = Npos->next;
	Latest->next = Npos->next;
	Npos->next = nullptr;
	delete Npos;
	num_latest = lpos;
	length--;
}

template <typename T>
Node<T>& List<T>::operator[](unsigned int pos)
{
	if (pos > length)
		throw OverFlow;

	if (!length)
		throw EmptyList;

	int lpos = pos;
	Node<T>* ret = First;

	if (pos >= num_latest)
	{
		pos -= num_latest;
		ret = Latest;
	}		
	for (unsigned int i = 0;i < pos;i++)
		ret = ret->next;

	Latest = ret;
	num_latest = lpos;

	return *ret;
}

template <typename T>
bool List<T>::is_empty()
{
	return length;
}



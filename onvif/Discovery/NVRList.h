#pragma once
#include "stdafx.h"
struct listElement
{
	void* object;
	listElement* next;
	listElement* prev;
};

class CNVRList
{
public:
	CNVRList(void);
	~CNVRList(void);

public:
	void Add(void* object);

	void Clear();

	BOOL Contains(void* object);

	void* Get(int index);
	void* GetNext();
	void* GetPrev();
	int IndexOf(void* object);

	BOOL IsEmpty();

	void Remove(int index);
	BOOL Remove(void* object);

	void Set(int index, void* object);
	int Size();

	void Insert(int index, void* object); // index after add
	void Delete(int index); // index after add

public:
	listElement* header;
	listElement* tailer;
	int count;

	listElement* GetPrivNext();

	listElement* currentPriv;
	listElement* current;
};

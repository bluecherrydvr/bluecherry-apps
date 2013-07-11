#pragma once
#include "stdafx.h"
#define ARRAY_BLOCK_SIZE 10

#include "NVRList.h"

struct arrayListElement
{
	void* blockAddress;
	int count;
	void* object[ARRAY_BLOCK_SIZE];
};

class CNVRArrayList
{
public:
	CNVRArrayList(void);
	~CNVRArrayList(void);

public:
	void Add(void* object);
	void Add(int index, void* object);

	void Clear();
	BOOL Contains(void* object);

	void* Get(int index);
	int IndexOf(void* object);
	BOOL IsEmpty();
	void Remove(int index);

	void Set(int index, void* object);
	int Size();
	BOOL readOnly;

private:
	int count;
#ifndef LINUX_NAS
	CRITICAL_SECTION criticalSection;
#else
	pthread_mutex_t criticalSection;
#endif
	void Lock();
	void UnLock();

	CNVRList privBlockList;
	arrayListElement* NewBlock();
	arrayListElement* FindPosition(int index, int& pos, int& blockIndex);
};

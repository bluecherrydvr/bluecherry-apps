#include "stdafx.h"
#include "NVRArrayList.h"
#include "UserType.h"

CNVRArrayList::CNVRArrayList(void)
{
	count = 0;
	//InitializeCriticalSection(&criticalSection);
	InitCritical(criticalSection);

	arrayListElement* elem = NewBlock();
	privBlockList.Add(elem);
}

CNVRArrayList::~CNVRArrayList(void)
{
	Clear();
}

void CNVRArrayList::Add(void* object)
{
	Lock();

	if(privBlockList.Size() == 0)
	{
		arrayListElement* elem = NewBlock();
		privBlockList.Add(elem);
	}

	//Add int tailer block
	arrayListElement* lastBlock = (arrayListElement*) (privBlockList.tailer->object);

	if(lastBlock->count < ARRAY_BLOCK_SIZE)
	{
		lastBlock->object[lastBlock->count] = object;
		lastBlock->count++;
		count++;

		UnLock();
		return;
	}

	//lastBlock is full
	arrayListElement* newElem = NewBlock();
	newElem->object[newElem->count] = object;
	newElem->count++;
	privBlockList.Add(newElem);
	count++;

	UnLock();
}

void CNVRArrayList::Add(int index, void* object)
{
	Lock();

	if(privBlockList.Size() == 0)
	{
		arrayListElement* elem = NewBlock();
		privBlockList.Add(elem);
	}

	int pos = 0;
	int blockIndex = 0;

	arrayListElement* block = FindPosition(index, pos, blockIndex);

	if(block == NULL)
	{
		UnLock();
		return;
	}

	if(block->count < ARRAY_BLOCK_SIZE) // don't full
	{
		for(int i=block->count; i>pos; i--)
			block->object[i] = block->object[i-1];
		
		block->object[pos] = object;
		block->count++;

		count++;

		UnLock();
		return;
	}

	// is full
	arrayListElement* blockAfter;
	blockAfter = NewBlock();
	blockAfter->count = 1;
	blockAfter->object[0] = object;
	privBlockList.Insert(blockIndex, blockAfter);
	count++;

	UnLock();
}

void CNVRArrayList::Clear()
{
	Lock();

//	if(Size() == 0)
//	{
//		UnLock();
//		return;
//	}

	privBlockList.current = NULL;

	arrayListElement* elem = (arrayListElement*) privBlockList.GetNext();

	while(elem)
	{
		for(int i=0; i<elem->count; i++)
			if(elem->object[i] != NULL)
				delete elem->object[i];
//		delete elem;

		elem = (arrayListElement*) privBlockList.GetNext();
	}

	privBlockList.Clear();

	count = 0;
	privBlockList.count = 0;
	privBlockList.header = NULL;
	privBlockList.tailer = NULL;

	UnLock();
}

BOOL CNVRArrayList::Contains(void* object)
{
	Lock();

	int index = IndexOf(object);

	if(index == -1)
	{
		UnLock();
		return FALSE;
	}

	UnLock();
	return TRUE;
}

void* CNVRArrayList::Get(int index)
{
	Lock();

	int pos = 0;
	int blockIndex = 0;
	if(index == -1)
	{
		UnLock();
		return NULL;
	}

	arrayListElement* block = FindPosition(index, pos, blockIndex);

	if(block != NULL)
	{
		UnLock();
		return block->object[pos];
	}

	UnLock();
	return NULL;
}

int CNVRArrayList::IndexOf(void* object)
{
	Lock();

	int index = -1;
	privBlockList.current = NULL;
	arrayListElement* block = (arrayListElement*) privBlockList.GetNext();

	while(block)
	{
		for(int i=0; i<block->count; i++)
		{
			index++;

			if(block->object[i] == object)
			{
				UnLock();
				return index;
			}
		}

		block = (arrayListElement*) privBlockList.GetNext();
	}

	UnLock();
	return -1;
}

BOOL CNVRArrayList::IsEmpty()
{
	return !count;
}

void CNVRArrayList::Remove(int index)
{
	Lock();

	int pos = 0;
	int blockIndex = 0;

	arrayListElement* block = FindPosition(index, pos, blockIndex);

	if(block == NULL)
	{
		UnLock();
		return;
	}

	if(block->object[pos] != NULL)
		delete block->object[pos];

	if(block->count > 0) // don't empty
	{
		for(int i=pos+1; i<block->count; i++)
			block->object[i-1] = block->object[i];
		
		block->count--;
		block->object[block->count] = NULL;
		
		count--;

		if(block->count == 0)
			privBlockList.Delete(blockIndex);

		UnLock();
		return;
	}

	// is empty
	privBlockList.Delete(blockIndex);
	UnLock();
}

void CNVRArrayList::Set(int index, void* object)
{
	Lock();

	int pos = 0;
	int blockIndex = 0;
	arrayListElement* block = FindPosition(index, pos, blockIndex);

	if(block != NULL)
	{
	//	if(block->object[pos] != NULL)
	//		delete block->object[pos];

		block->object[pos] = object;
	}

	UnLock();
}

int CNVRArrayList::Size()
{
	return count;
}

arrayListElement* CNVRArrayList::NewBlock()
{
	arrayListElement* elem = new arrayListElement;

	for(int i=0; i<ARRAY_BLOCK_SIZE; i++)
		elem->object[i] = NULL;
		
	elem->blockAddress = elem->object;
	elem->count = 0;

	return elem;
}

arrayListElement* CNVRArrayList::FindPosition(int index, int& pos, int& blockIndex)
{
	pos = -1;
	blockIndex = -1;
	listElement * elem = privBlockList.header;

	if(elem == NULL)
		return NULL;
	
	arrayListElement* block = NULL;

	do
	{
		blockIndex++;
		block = (arrayListElement*) (elem->object);
		if(block == NULL)
			return NULL;
		if(block->count > index)
		{
			pos = index;
			return block;
		}

		index -= block->count;
		elem = elem->next;
	} while(elem);

	return NULL;
}

void CNVRArrayList::Lock()
{
	EnterCritical(criticalSection);
}

void CNVRArrayList::UnLock()
{
	LeaveCritical(criticalSection);
}
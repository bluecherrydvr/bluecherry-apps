#include "stdafx.h"
#include "NVRList.h"

CNVRList::CNVRList(void)
{
	count = 0;
	header = NULL;
	tailer = NULL;
	current = NULL;
	currentPriv = NULL;
}

CNVRList::~CNVRList(void)
{
}

void CNVRList::Add(void* object)
{
	listElement* newElement = new listElement();
	newElement->object = object;

	if(count==0)
	{
		newElement->next = NULL;
		newElement->prev = NULL;
		header = newElement;
		tailer = newElement;
	}
	else
	{
		tailer->next = newElement;
		newElement->next = NULL;
		newElement->prev = tailer;
		tailer = newElement;
	}

	count++;
}

void CNVRList::Clear()
{
	if(count==0)
	{
		count = 0;
		header = NULL;
		tailer = NULL;
		current = NULL;
		currentPriv = NULL;

		return;
	}

	currentPriv = NULL;
	listElement* elem;
	listElement* nextElem;

	elem = GetPrivNext();

	for(int i=0; i<Size(); i++)
	{
		nextElem = GetPrivNext();

		if(elem != NULL)
		{
			if(elem->object != NULL)
				delete elem->object;

			delete elem;
		}

		if(nextElem == NULL)
			break;

		elem = nextElem;
	}

	count = 0;
	header = NULL;
	tailer = NULL;
	current = NULL;
	currentPriv = NULL;
}

BOOL CNVRList::Contains(void* object)
{
	listElement* elem;
	currentPriv = NULL;

	for(int i=0; i<Size(); i++)
	{
		elem = GetPrivNext();
		
		if(elem == NULL)
			continue;

		if(object == elem->object)
			return TRUE;
	}

	return FALSE;
}

void* CNVRList::Get(int index)
{
	if(index >= Size())
		return NULL;

	listElement* elem = NULL;
	currentPriv = NULL;

	for(int i=0; i<=index; i++)
		elem = GetPrivNext();

	if(elem == NULL)
		return NULL;

	return elem->object;
}

void* CNVRList::GetNext()
{
	if(Size() == 0)
		return NULL;

	if(current == NULL)
	{
		current = header;
		return header->object;
	}

	listElement* elem = current->next;
	current = current->next;
	if(elem == NULL)
		return NULL;
	
	return elem->object;
}

void* CNVRList::GetPrev()
{
	if(Size() == 0)
		return NULL;

	if(current == NULL)
		return NULL;

	listElement* elem = current->prev;
	current = current->prev;
	if(elem == NULL)
		return NULL;

	return elem->object;
}

int CNVRList::IndexOf(void* object)
{
	if(Size() == 0 || object == NULL)
		return -1;

	listElement* elem = NULL;

	currentPriv = NULL;

	int i;

	for(i=0; i<Size(); i++)
	{
		elem = GetPrivNext();

		if(elem == NULL)
			return -1;

		if(elem->object == object)
			break;
	}

	if(i == Size())
		return -1;

	return i;
}

BOOL CNVRList::IsEmpty()
{
	return !count;
}

void CNVRList::Remove(int index)
{
	if(index >= Size())
		return;

	listElement* elem = NULL;
	listElement* nextElem = NULL;
	listElement* prevElem = NULL;

	currentPriv = NULL;

	for(int i=0; i<=index; i++)
		elem = GetPrivNext();

	if(elem == NULL)
		return;

	nextElem = elem->next;
	prevElem = elem->prev;

	if(prevElem != NULL)
		prevElem->next = nextElem;

	if(nextElem != NULL)
		nextElem->prev = prevElem;

	if(elem == tailer) // elem is tailer
		tailer = prevElem;
	else if(elem == header)
		header = nextElem;

	if(elem->object)
		delete elem->object;

	delete elem;

	count--;
}

BOOL CNVRList::Remove(void* object)
{
	int index = IndexOf(object);

	if(index == -1)
		return FALSE;

	Remove(index);

	return TRUE;
}
	

void CNVRList::Set(int index, void* object)
{
	if(index >= Size())
		return;

	listElement* elem = NULL;
	currentPriv = NULL;

	for(int i=0; i<=index; i++)
		elem = GetPrivNext();

	if(elem == NULL)
		return;

	delete elem->object;
	elem->object = object;
}

int CNVRList::Size()
{
	return count;
}

listElement* CNVRList::GetPrivNext()
{
	if(Size() == 0)
		return NULL;

	if(currentPriv == NULL)
	{
		currentPriv = header;
		return header;
	}

	listElement* elem = currentPriv->next;
	currentPriv = currentPriv->next;

	return elem;
}

void CNVRList::Insert(int index, void* object)
{
	if(Size() == 0)
	{
		if(index == -1) // first
			Add(object);

		return;
	}
	
	listElement* elem = NULL;
	if(index == -1)
	{
		elem = (listElement*)Get(0);
		listElement* newElem = new listElement;
		newElem->object = object;
		header = newElem;
		newElem->next = elem;
		newElem->prev = NULL;

		if(elem != NULL)
			elem->prev = newElem;

		return;
	}

	currentPriv = NULL;
	for(int i=0; i<=index; i++)
		elem = GetPrivNext();

	if(elem == NULL)
		return;

	listElement* newElem = new listElement;
	listElement* nextElem = elem->next;

	newElem->next = nextElem;
	newElem->prev = elem;

	if(nextElem == NULL)
		tailer = newElem;
	else
		nextElem->prev = newElem;
}

void CNVRList::Delete(int index)
{
	if(Size() == 0)
		return;
		
	currentPriv = NULL;
	listElement* elem = NULL;
	for(int i=0; i<=index; i++)
		elem = GetPrivNext();

	if(elem == NULL)
		return;

	listElement* nextElem = elem->next;
	listElement* prevElem = elem->prev;

	if(elem->object != NULL)
		delete elem->object;
	delete elem;
	elem = NULL;

	count--;

	if(prevElem == NULL)
	{
		header = nextElem;

		if(nextElem == NULL)
		{
			tailer = NULL;
		}
		else
			nextElem->prev = NULL;

		return;
	}
	else if(nextElem == NULL)
	{
		tailer = prevElem;
		prevElem->next = NULL;
		
		return;
	}

	prevElem->next = nextElem;
	nextElem->prev = prevElem;
}
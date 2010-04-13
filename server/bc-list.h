/*
 * Copyright (C) 2010 Bluecherry, LLC
 *
 * Confidential, all rights reserved. No distribution is permitted.
 */

#ifndef __BC_LIST_H
#define __BC_LIST_H

#include <stddef.h>

struct bc_list_struct {
	struct bc_list_struct *next, *prev;
};

#define BC_DECLARE_LIST(name) \
	struct bc_list_struct name = { &(name), &(name) }

static inline void bc_list_add(struct bc_list_struct *new,
			       struct bc_list_struct *head)
{
	head->next->prev = new;
	new->next = head->next;
	new->prev = head;
	head->next = new;
}

static inline void bc_list_del(struct bc_list_struct *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	item->next = NULL;
	item->prev = NULL;
}

static inline int bc_list_empty(const struct bc_list_struct *head)
{
	return head->next == head;
}

#define bc_list_entry(ptr, type, member) ({ \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

#define bc_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#endif /* __BC_LIST_H */

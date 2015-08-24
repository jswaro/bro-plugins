/*
 * Copyright (c) 2011-2015 James Swaro
 * Copyright (c) 2011-2015 Internetworking Research Group, Ohio University
 */

#ifndef DLIST_H_
#define DLIST_H_

#include "stddef.h"

#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define dlist_for_each(iter, head) \
    for (iter = head->next; iter != head; iter = iter->next)

#define dlist_for_each_reverse(iter, head) \
    for (iter = head->prev; iter != head; iter = iter->prev)

#define dlist_for_each_entry(iter, head, elem) \
    for (iter = container_of((head)->next, typeof(*iter), elem); \
        &(iter)->elem != head; \
        iter = container_of(iter->elem.next, typeof(*iter), elem))

#define dlist_for_each_entry_reverse(iter, head, elem) \
    for (iter = container_of((head)->prev, typeof(*iter), elem); \
        &(iter)->elem != head; \
        iter = container_of(iter->elem.prev, typeof(*iter), elem))

#define dlist_for_each_entry_safe(iter, curr, head, elem) \
    for (iter = container_of((head)->next, typeof(*iter), elem), \
            curr = container_of((iter)->elem.next, typeof(*iter), elem); \
        &(iter)->elem != head; \
        iter = curr, \
        curr = container_of(iter->elem.next, typeof(*iter), elem))

#define dlist_for_each_entry_reverse_safe(iter, curr, head, elem) \
    for (iter = container_of((head)->prev, typeof(*iter), elem), \
            curr = container_of((iter)->elem.prev, typeof(*iter), elem); \
        &(iter)->elem != head; \
        iter = curr, \
        curr = container_of(iter->elem.prev, typeof(*iter), elem))

struct dlist_entry {
    struct dlist_entry *next;
    struct dlist_entry *prev;
};

static inline int dlist_empty(struct dlist_entry *head)
{
    return head->next == head;
}

static inline void dlist_insert_next(
        struct dlist_entry *head,
        struct dlist_entry *entry)
{
    /* modify entry */
    entry->next = head->next;
    entry->prev = head;

    /* modify head contents */
    head->next->prev = entry;
    head->next = entry;
}

static inline void dlist_insert_prev(
        struct dlist_entry *head,
        struct dlist_entry *entry)
{
    return dlist_insert_next(head->prev, entry);
}

static inline void dlist_insert_head(
        struct dlist_entry *head,
        struct dlist_entry *entry)
{
    dlist_insert_next(head, entry);
}

static inline void dlist_insert_tail(
        struct dlist_entry *head,
        struct dlist_entry *entry)
{
    dlist_insert_prev(head, entry);
}

static inline void dlist_remove(struct dlist_entry *entry)
{
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->next = entry->prev = entry;
}

static inline void dlist_sorted_insert(
        struct dlist_entry *head,
        struct dlist_entry *entry,
        int (*compare)(struct dlist_entry *, struct dlist_entry *))
{
    struct dlist_entry *current;
    int result;

    dlist_for_each(current, head)
    {
        result = compare(current, entry);

        if (result < 0) {
            dlist_insert_prev(current, entry);
            return;
        }
    }

    dlist_insert_tail(head, entry);
}

static inline void dlist_sorted_insert_reverse(
        struct dlist_entry *head,
        struct dlist_entry *entry,
        int (*compare)(struct dlist_entry *, struct dlist_entry *))
{
    struct dlist_entry *current;
    int result;

    dlist_for_each_reverse(current, head)
    {
        result = compare(current, entry);

        if (result > 0) {
            dlist_insert_next(current, entry);
            return;
        }
    }

    dlist_insert_head(head, entry);
}

#define INIT_DLIST(head) { &head, &head }

static inline void dlist_init(struct dlist_entry *head)
{
    head->next = head->prev = head;
}

static inline struct dlist_entry *dlist_front(struct dlist_entry *head)
{
    return head->next;
}

static inline struct dlist_entry *dlist_back(struct dlist_entry *head)
{
    return head->prev;
}

static inline struct dlist_entry *dlist_remove_entry_reverse(
        struct dlist_entry *head,
        void *to_find,
        int (*compare)(struct dlist_entry *, void *))
{
    struct dlist_entry *current;

    dlist_for_each_reverse(current, head)
    {
        if (compare(current, to_find)) {
            dlist_remove(current);
            return current;
        }
    }

    return NULL;
}

static inline struct dlist_entry *dlist_remove_entry(
        struct dlist_entry *head,
        void *to_find,
        int (*compare)(struct dlist_entry *, void *))
{
    struct dlist_entry *current;

    dlist_for_each(current, head)
    {
        if (compare(current, to_find)) {
            dlist_remove(current);
            return current;
        }
    }

    return NULL;
}


static inline void dlist_splice(
		struct dlist_entry *from_list,
		struct dlist_entry *to_list)
{
	/* connect the tail of from_list to start of to_list */
	to_list->next->prev = from_list->prev;
	from_list->prev->next = to_list->next;

	/* connect the head of from_list to start of to_list */
	from_list->next->prev = to_list;
	to_list->next = from_list->next;
}

static inline void dlist_splice_tail(
		struct dlist_entry *from_list,
		struct dlist_entry *to_list)
{
	dlist_splice(from_list, to_list->prev);
}

static inline struct dlist_entry *dlist_pop(
		struct dlist_entry *head)
{
	struct dlist_entry *ret;

	if (dlist_empty(head))
		return NULL;

	ret = head->next;

	dlist_remove(ret);

	return ret;
}

#endif /* DLIST_H_ */


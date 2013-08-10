#ifndef __LIST_H
#define __LIST_H

#include <stddef.h>

#define LIST_POISON1 ((void*)0x00100100)
#define LIST_POISON2 ((void*)0x00200200)

struct list_head {
    struct list_head *prev, *next;
};

#define LIST_HEAD_INIT(name) {&(name), &(name)}
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
}while(0)

static inline void __list_add(struct list_head * add,
                       struct list_head * prev,
                       struct list_head * next)
{
   next->prev = add;
   add->next = next;
   add->prev = prev;
   prev->next = add;
}

static inline void list_add(struct list_head * add,
                     struct list_head * head)
{
    __list_add(add, head, head->next);
}

static inline void list_add_tail(struct list_head * add,
                          struct list_head * head)
{
    __list_add(add, head->prev, head);
}

static inline void __list_del(struct list_head * prev,
                       struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head * entry)
{
    __list_del(entry->prev, entry->next);
}

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline int list_empty_careful(const struct list_head *head)
{
    struct list_head * next = head->next;
    return (next == head) && (next == head->prev);
}

#define list_entry(ptr, type, member) \
    ((type*)((char*)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, pnext, head) \
    for (pos = (head)->next, pnext = pos->next; \
         pos != (head); \
         pos = pnext, pnext = pos->next)

#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
        &pos->member != (head); \
        pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member), \
        n = list_entry(pos->member.next, typeof(*pos), member); \
        &pos->member != (head); \
        pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * @brief node for hash table
 */

struct hlist_node {
    struct hlist_node *next, **pprev;
};

struct hlist_head {
    struct hlist_node *first;
};

#define HLIST_HEAD_INIT {.first = NULL}
#define HLIST_HEAD(name) struct hlist_head name = {.first=NULL}
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

static inline void INIT_HLIST_NODE(struct hlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}

static inline int hlist_unhashed(const struct hlist_node *h)
{
    return !h->pprev;
}

static inline int hlist_empty(const struct hlist_head *h)
{
    return !h->first;
}

static inline void __hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}

static inline void hlist_del(struct hlist_node *n)
{
    __hlist_del(n);
    n->next = (struct hlist_node*)LIST_POISON1;
    n->pprev  = (struct hlist_node**)LIST_POISON2;
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

/* next must be NULL */
static inline void hlist_add_before(struct hlist_node *n,
                                    struct hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

static inline void hlist_add_after(struct hlist_node *n,
                                   struct hlist_node *next)
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;

    if (next->next)
        next->next->pprev = &next->next;
}

/**
 * @brief Move a list from one list head to another. Fixup the pprev
 *      reference of the first entry if it exists.
 * @notice 必须要确保newlist上没有东西，否则就丢失了。
 */
static inline void hlist_move_list(struct hlist_head *old,
                                   struct hlist_head *newlist)
{
    newlist->first = old->first;
    if (newlist->first)
        newlist->first->pprev = &newlist->first;
    old->first = NULL;
}

#define hlist_entry(ptr, type, member) \
    ((type*)((char*)(ptr)-(unsigned long)(&((type *)0)->member)))

#define hlist_for_each(pos, head) \
    for (pos=(head)->first; pos; pos=pos->next)

#define hlist_for_each_safe(pos, n, head) \
    for (pos=(head)->first; pos && ({n=pos->next;1;}); pos=n)

#define hlist_for_each_entry(tpos, pos, head, member) \
    for (pos=(head)->first; \
        pos && ({tpos=hlist_entry(pos, typeof(*tpos), member);1;}); \
        pos=pos->next)

/* iterate over a hlist continuing after current point */
#define hlist_for_each_continue(tpos, pos, member)  \
    for (pos=(pos)->next; \
        pos && ({tpos=hlist_entry(pos, typeof(*tpos), member);1;}); \
        pos=pos->next)

/* iterate over a hlist continuing from current point */
#define hlist_for_each_entry_from(tpos, pos, member)  \
    for (; \
        pos && ({tpos=hlist_entry(pos, typeof(*tpos), member);1;}); \
        pos=pos->next)

#define hlist_for_each_entry_safe(tpos, pos, n, head, member) \
    for (pos=(head)->first; \
        pos && ({n=pos->next;1;}) && \
        ({tpos=hlist_entry(pos, typeof(*tpos), member);1;}); \
        pos = n)


#endif

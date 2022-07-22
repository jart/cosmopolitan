#ifndef COSMOPOLITAN_THIRD_PARTY_QUICKJS_LIST_H_
#define COSMOPOLITAN_THIRD_PARTY_QUICKJS_LIST_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/* clang-format off */

struct list_head {
    struct list_head *prev;
    struct list_head *next;
};

#define LIST_HEAD_INIT(el) { &(el), &(el) }

/* return the pointer of type 'type *' containing 'el' as field 'member' */
#define list_entry(el, type, member) \
    ((type *)((uint8_t *)(el) - offsetof(type, member)))

static inline void init_list_head(struct list_head *head)
{
    head->prev = head;
    head->next = head;
}

/* insert 'el' between 'prev' and 'next' */
static inline void __list_add(struct list_head *el, 
                              struct list_head *prev, struct list_head *next)
{
    prev->next = el;
    el->prev = prev;
    el->next = next;
    next->prev = el;
}

/* add 'el' at the head of the list 'head' (= after element head) */
static inline void list_add(struct list_head *el, struct list_head *head)
{
    __list_add(el, head, head->next);
}

/* add 'el' at the end of the list 'head' (= before element head) */
static inline void list_add_tail(struct list_head *el, struct list_head *head)
{
    __list_add(el, head->prev, head);
}

static inline void list_del(struct list_head *el)
{
    struct list_head *prev, *next;
    prev = el->prev;
    next = el->next;
    prev->next = next;
    next->prev = prev;
    el->prev = NULL; /* fail safe */
    el->next = NULL; /* fail safe */
}

static inline int list_empty(struct list_head *el)
{
    return el->next == el;
}

#define list_for_each(el, head) \
  for(el = (head)->next; el != (head); el = el->next)

#define list_for_each_safe(el, el1, head)                \
    for(el = (head)->next, el1 = el->next; el != (head); \
        el = el1, el1 = el->next)

#define list_for_each_prev(el, head) \
  for(el = (head)->prev; el != (head); el = el->prev)

#define list_for_each_prev_safe(el, el1, head)           \
    for(el = (head)->prev, el1 = el->prev; el != (head); \
        el = el1, el1 = el->prev)

/* clang-format on */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_QUICKJS_LIST_H_ */

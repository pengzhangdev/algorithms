/**
 * The basic list
 */

#ifndef _ALG_LIST_H
#define _ALG_LIST_H

struct alg_list_node {
    struct alg_list *prev;
    struct alg_list *next;
};


#define ALG_LIST_INIT(head) \
    do {                             \
        (head)->prev = (head);       \
        (head)->next = (head);       \
    }while(0)                        \

#define GET_LIST_DATA(type, mem, pos)           \
    ((type *)((unsigned int)pos - (unsigned long)(&((type*)0)->mem)))   \

static inline void alg_list_add (struct alg_list_node *ahead,
                             struct alg_list_node *back,
                             struct alg_list_node *add)
{
    ahead->next = add;
    add->prev = ahead;
    add->next = back;
    back->prev = add;
}

/**
 * @add, the node to add
 * @to,  the node to be added ahead
 */
static inline void alg_list_add_ahead(struct alg_list_node *add,
                                 struct alg_list_node *to)
{
    alg_list_add(to->prev, to, add);
}

/**
 * @add,  the node to add
 * @to,   the node to be added back
 */
static inline void alg_list_add_back(struct alg_list_node *add,
                                     struct alg_list_node *to)
{
    alg_list_add(to, to->next, add);
}

/**
 * del node between @before and @back
 */
static inline void alg_list_del_(struct alg_list_node *before,
                                 struct alg_list_node *back)
{
    before->next = back;
    back->prev = before;
}

/**
 * del node @del
 */
static inline void alg_list_del(struct alg_list_node *del)
{
    alg_list_del_(del->prev, del->next);
    del->prev = NULL;
    del->next = NULL;
}

/**
 * treat the list as stack, and add @add to the first of @head
 */
static inline void alg_list_push(struct alg_list_node *head,
                                 struct alg_list_node *add)
{
    alg_list_add(head, head->next, add);
}

/**
 * treat the list as stack, del the first of @head and return ;
 */
static inline struct alg_list_node* alg_list_pop(struct alg_list_node *head) {
    struct alg_list_node *node = head->next;
    alg_list_del(node);
    return node;
}

/**
 * treat the list as queue, add the @add the the tail of @head
 */
static inline void alg_list_enqueue(struct alg_list_node *head,
                                    struct alg_list_node *add)
{
    alg_list_add(head, head->prev, add);
}

/**
 * treat the list as queue, del the first of @head and return;
 */
static inline struct alg_list_node *alg_list_dequeue(struct alg_list_node *head)
{
    struct alg_list_node *node = head->next;
    alg_list_del(node);
    return node;
}

/**
 * check whether the head is empty
 */
static inline int alg_list_is_empty(struct alg_list_node *head)
{
    return (head->next == head);
}

/**
 * check whether the @node is the tail of @head
 */
static inline int alg_list_is_tail(struct alg_list_node *head,
                                   struct alg_list_node *node)
{
    return (head->prev == node);
}

#endif

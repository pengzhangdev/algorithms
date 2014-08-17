#ifndef __RADIX_TREE_H__
#define __RADIX_TREE_H__

typedef void (*destroy_t)(void *data);

struct rt_node_t;
struct rt_t;

typedef struct rt_node_t rt_node_t;
typedef struct rt_t rt_t;

enum {
    RT_SEARCH_FULL = 1,
    RT_SEARCH_PREFIX,
};

/*
 * rules: using empty edge as the end of toast,
 *        so that we won't merge "toast" and "er"" as edge "toaster"
 *      toast
 *      /  \
 *     er  [node(end:1)]
 */

struct rt_node_t {              /* children sets */
    char *key;
    void *data;
    int matched;
    int num;                    /* children number */
    int end;                    
    rt_node_t *parent;
    rt_node_t *next_sibling;    /* every time insert at the list head */
    rt_node_t *prev_sibling;
    rt_node_t *children;
};

struct rt_t {
    rt_node_t *root;
    destroy_t destroy;
};

/**
 * create radix tree with custom destroy function.
 */
rt_t *rt_create(destroy_t destroy);

/**
 * destroy radix tree, free all data and nodes
 */
int rt_destroy(rt_t *tree);

/**
 * travese tree, if found return 0, otherwise return -1 and set the to the last one
 * Then we can add new edge or split edge in @node if we insert a pair{key, value}
 */
int rt_traverse(rt_t *tree, char *key, rt_node_t **node);

/**
 * find key, prefix or all keys.
 */
rt_node_t *rt_search(rt_t *tree, char *key, int prefix);

/**
 * Insert pair{key, value} to radix tree @tree,
 * First, try to search the key.
 */ 
int rt_insert(rt_t *tree, char *key, void *data, int replace);

/**
 * delete key from tree.
 */
int rt_delete(rt_t *tree, char *key);

/**
 * for debug, dump all keys
 */
void rt_dump(rt_t *tree);

#endif

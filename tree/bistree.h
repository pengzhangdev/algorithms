#ifndef BISTREE_H
#define BISTREE_H

#include <stdio.h>
#include "bitree.h"

typedef struct bitree_tree BisTree;
typedef struct bitree_node BiTreeNode;

typedef struct AvlNode_ {
    void *data;
    int hidden;
    int factor;
} AvlNode;

enum avl_balance_state_{
    AVL_RGT_HEAVY = -1,
    AVL_BALANCED = 0,
    AVL_LET_HEAVY = 1,
};

/**
 * init binary search tree
 * @tree        global BisTree struct
 * @compare     compare key1 and key2.
 *              return 1 on key1 > key2.
 *              return 0 on key1 == key2.
 *              return -1 on key1 < key2.
 * @destroy     destroy private data in BisTree.
 */
void bistree_init(BisTree *tree, int (*compare)(const void *key1, const void *key2),
                  void (*destroy)(void *data));

/**
 * destroy binary search tre
 * @tree        global BisTree struct
 */
void bistree_destroy(BisTree *tree);

/**
 * insert @data to @tree
 * @data        private data
 * @tree        global binary search tree struct
 * @return      return 0 on success otherwise return -1
 */
int bistree_insert(BisTree *tree, const void *data);

/**
 * remove @data from @tree
 */
int bistree_remove(BisTree *tree, const void *data);


/**
 * lookup data
 */
int bistree_lookup(BisTree *tree, void **data);

static inline size_t bistree_size(BisTree *tree)
{
    return tree->size;
}

#endif

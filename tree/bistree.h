#ifndef BISTREE_H
#define BISTREE_H

#include <stdio.h>
#include "bitree.h"

typedef bitree_tree isTree;

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
int bistree_insert(BisTree *tree, const char *data);

/**
 * remove @data from @tree
 */
int bistree_remove(BisTree *tree, const char *data);

#endif

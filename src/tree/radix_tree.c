#include "radix_tree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NDEBUG
#include <assert.h>

#define is_leaf(node)                           \
    (node->num <= 0 ? 1 : 0)


static int rt_node_free(rt_node_t *node, destroy_t destroy);
static rt_node_t *rt_node_malloc(char *key, void *data);

static rt_node_t *rt_node_malloc(char *key, void *data)
{
    rt_node_t *node = (rt_node_t *)malloc(sizeof(rt_node_t));
    char *node_key = (key == NULL? NULL : strdup (key));
    if (node == NULL)
        goto exit;

    node->key = node_key;
    node->data = data;
    node->num = 0;
    node->matched = 0;
    node->end = 0;
    node->parent = NULL;
    node->next_sibling = NULL;
    node->prev_sibling = NULL;
    node->children = NULL;

exit:
    return node;
}

static int rt_node_merge(rt_node_t *node, destroy_t destroy)
{
    /* Ok, we should merge this node and it's child */
    /* Merge key and replace data with child. */
    size_t size = strlen(node->key) + strlen(node->children->key) + 1;
    char *key = (char *)malloc(size);
    memset(key, '\0', size);
    snprintf(key, size, "%s%s", node->key, node->children->key);

    free(node->key);
    node->key = key;
    if (node->data && destroy)
        destroy(node->data);
    node->data = node->children->data;
    node->num --;
    assert(node->num == 0);

    node->children->parent = NULL;
    rt_node_free(node->children, destroy);
    node->children = NULL;

    assert(node->parent->num != 0);
    if (node->parent->num == 1) {
        rt_node_merge(node->parent, destroy);
    }

    return 0;
}

/**
 * return the parent of splited node
 * new : parent
 * old : child
 */
static rt_node_t *rt_node_split(rt_node_t *node, int index)
{
    size_t size = strlen(node->key + index);
    char *new_key = (char *)malloc(index + 1);
    rt_node_t *new_node = NULL;

    //assert(index != strlen(node->key));
    memcpy(new_key, node->key, index);

    if (size != 0) {
        memcpy(node->key, node->key + index, size);
        *(node->key + size) = '\0';
    }
    else {
        //memset(node->key, '\0', strlen(node->key));
        free(node->key);
        node->key = (char *)malloc(0);
        node->end = 1;
    }

    *(new_key + index) = '\0';
    new_node = rt_node_malloc(new_key, NULL);

    if (node->parent->children == node) {
        node->parent->children = new_node;
    }
    new_node->parent = node->parent;
    new_node->next_sibling = node->next_sibling;
    new_node->prev_sibling = node->prev_sibling;
    if (node->next_sibling)
        node->next_sibling->prev_sibling = new_node;
    if (node->prev_sibling)
        node->prev_sibling->next_sibling = new_node;
    new_node->num ++;
    new_node->children = node;
    node->parent = new_node;
    node->next_sibling = NULL;
    node->prev_sibling = NULL;

    /* No need to merge new children,
     * because if it should be merged
     * before split.
     */

    free(new_key);
    return new_node;
}

static int rt_node_free(rt_node_t *node, destroy_t destroy)
{
    rt_node_t *parent = node->parent;
    rt_node_t *prev = NULL;
    rt_node_t *next = NULL;
    if (!is_leaf(node)) {
        printf("Not a leaf, ignore to free!\n");
        return -1;
    }

    //printf("aa %p %s %d\n", node, node->key, node->end);
    if (node->key)
        free(node->key);

    if (node->data && destroy)
        destroy(node->data);

    prev = node->prev_sibling;
    next = node->next_sibling;
    if (prev != NULL) {
        prev->next_sibling = next;
    }
    if (next != NULL) {
        next->prev_sibling = prev;
    }

    /* Maybe we just free it. */
    if (prev == NULL && parent) {
        /* make sure parent->children != NULL */
        parent->children = next;
    }

    if (parent)
        parent->num --;

    free(node);

    if (parent && parent->num == 1) { // No case to be 0.
        rt_node_merge(parent, destroy);
    }

    return 0;
}

/**
 * return number of element matched.
 */
static int rt_is_prefix(char *key1, char *key2)
{
    int ret = 0;
    int matched = 0;
    size_t size_key1 = strlen(key1);
    size_t size_key2 = strlen(key2);
    size_t size = size_key1 < size_key2? size_key1 : size_key2;
    int index = 0;

    if (key1[0] != key2[0])
        goto exit;

    ret = memcmp(key1, key2, size);

    if (ret == 0) {
        ret = size;
        goto exit;
    }

    for (index = 0; index < size; index++) {
        if (key1[index] == key2[index])
            matched ++;
        else
            break;
    }
    ret = matched;

exit:
    return ret;
}

static int rt_destroy_internal(rt_node_t *node, destroy_t destroy)
{
    int ret = 0;
    rt_node_t *parent = node->parent;
    rt_node_t *next = node->next_sibling;

    printf(" destroy %s\n", node->key);
    
    if (is_leaf(node)) {
        /* removing start at leaf, from left to right */
        node->parent = NULL;
        rt_node_free(node, destroy);
        if (next == NULL && parent != NULL) {
            parent->children = NULL;
            parent->num = 0;
            rt_destroy_internal(parent, destroy);
        }
        else if (next != NULL){
            rt_destroy_internal(next, destroy);
        }
        else {
            return ret;
        }
    }
    else {
        rt_destroy_internal(node->children, destroy);
    }

    return ret;
}

rt_t *rt_create(destroy_t destroy)
{
    rt_t *tree = (rt_t *)malloc(sizeof(rt_t));
    rt_node_t *node = rt_node_malloc(NULL, 0);

    if (tree == NULL || node == NULL)
        goto bail;

    tree->root = node;
    tree->destroy = destroy;
    return tree;

bail:
    if (tree)
        free(tree);
    return tree;
}

int rt_destroy(rt_t *tree)
{
    rt_node_t *root = tree->root;
    destroy_t destroy = tree->destroy;
    rt_destroy_internal(root, destroy);
    free(tree);
    return 0;
}

/**
 * only one of the children will match the first element of key,
 * because if there is more than one, the same ones will be merged
 */
static int rt_traverse_internal(rt_node_t *node, char *key, rt_node_t **ret_node)
{
    int ret = 0;
    int matched = 0;
    rt_node_t *children = node->children;
    rt_node_t *found = NULL;

    if (children == NULL) {
        goto exit;
    }

    for (; children != NULL;
         children = children->next_sibling) {
        matched = rt_is_prefix(key, children->key);

        if (matched == 0) {
            /* Not found */
            continue;
        }

        if (matched == strlen(key)) {
            /* found! */
            found = children;
            ret = matched;
            goto exit;
        }

        /* else first some elements matched */
        found = children;
        ret = matched;
    }

exit:
    /*
     * case 1: the key matched some at the leaf, return the leaf and matched elements num
     * case 2: the key matched some at the node not a leaf, return the node and matched elements num.
     * case 3: the key matched all at the node not a leaf, return the node and matched elements num. That means, user request to find all data that prefix matched with @key
     */

    /* case 2 */
    if (matched == 0) {
        *ret_node = node;
        (*ret_node)->matched = matched;
    }
    else {
        /* case 1 */
        *ret_node = found;
        (*ret_node)->matched = matched;
    }

    if (matched > 0
        && matched != strlen(key)
        && !is_leaf(found)) {
        /*  found children */
        int m = rt_traverse_internal(found, key + matched, ret_node);
        /* Not found at children, so case 2 */
        if (m == 0) {
            (*ret_node)->matched = matched;
        }

        ret += m;
    }

    return ret;
}

int rt_traverse(rt_t *tree, char *key, rt_node_t **node)
{
    rt_node_t *root = tree->root;

    return rt_traverse_internal(root, key, node);
}

rt_node_t *rt_search(rt_t *tree, char *key, int prefix)
{
    rt_node_t *node = NULL;
    rt_node_t *target_node = NULL;
    int ret = rt_traverse(tree, key, &node);

    assert(node != NULL);

    if (node->matched > 0 && !is_leaf(node)
        && prefix == RT_SEARCH_PREFIX) {
        /* find all data with prefix @key */
        assert(ret == strlen(key));
        target_node = node;
    }

    if (ret == strlen(key) && prefix == RT_SEARCH_FULL) {
        /* find the requested data */
        if (!is_leaf(node)) {
            rt_node_t *index = NULL;
            for(index = node->children;
                index != NULL;
                index = index->next_sibling) {
                if (index->end == 1) {
                    target_node = index;
                    break;
                }
            }
        }
        else {
            target_node =  node;
        }
    }

    return target_node;
}

/*
 * 4 cases:
 * case 1: add new child node at root
 * case 2: is a leaf, and matched, try to replace.
 * case 3: is not a leaf, and node matches some of the @key, just add as the children.
 * case 4: is not a leaf, and some matches some of the @key, just split the node and add as the children of new node
 */
int rt_insert(rt_t *tree, char *key, void *data, int replace)
{
    int ret = -1;
    int matched = 0;
    rt_node_t *node = NULL;
    matched = rt_traverse(tree, key, &node);

    rt_node_t *new = NULL;

    if (node == tree->root) {
        new = rt_node_malloc(key, data);
        rt_node_t *child = node->children;
        new->next_sibling = child;
        if (child)
            child->prev_sibling = new;
        new->parent = node;
        node->children = new;
        node->num++;
        ret = 0;
        goto exit;
    }

    assert(node != NULL);
    if (matched == strlen(key)
        && matched == strlen(node->key)
        && is_leaf(node)
        && replace) {
        /* found node and replace */
        new = rt_node_malloc(node->key, data);
        new->num = node->num;
        new->parent = node->parent;
        new->next_sibling = node->next_sibling;
        new->prev_sibling = node->prev_sibling;
        if (node->next_sibling)
            node->next_sibling->prev_sibling = new;
        if (node->prev_sibling)
            node->prev_sibling->next_sibling = new;

        if (node->parent->children == node) {
            node->parent->children = new;
        }
        rt_node_free(node, tree->destroy);
        ret = 0;
        goto exit;
    }

    if (!is_leaf(node)
        && matched == strlen(node->key)
        && matched < strlen(key)){
        rt_node_t *child = node->children;
        new = rt_node_malloc(key + matched, data);
        new->next_sibling = child;
        if (child)
            child->prev_sibling = new;
        new->parent = node;
        node->num++;
        ret = 0;
        goto exit;
    }

    /* if (is_leaf(node) */
    /*     && ) { */

    /* } */

    if (matched != strlen(key)) {
        /* split the node and add new node to parent */
        rt_node_t *parent = rt_node_split(node, node->matched);
        new = rt_node_malloc(key + matched, data);
        new->next_sibling = parent->children;
        parent->children->prev_sibling = new;
        new->parent = parent;
        parent->children = new;
        parent->num++;
        ret = 0;
        goto exit;
    }

exit:
    if (ret == -1 && new)
        rt_node_free(new, tree->destroy);

    return ret;
}

/*
 * if @key is matched prefix, remove all, else remove the leaf with key.
 */
int rt_delete(rt_t *tree, char *key)
{
    rt_node_t *node = rt_search(tree, key, RT_SEARCH_FULL);
    if (node == NULL)
        /* No node matched the key found */
        return -1;

    assert(is_leaf(node));
    return rt_node_free(node, tree->destroy);
}

/* for debug */

static void rt_node_dump_info(rt_node_t *node)
{
    printf("key: %s, childre: %d\n", node->key, node->num);
}

static void rt_node_dump(rt_node_t *node)
{
    rt_node_t *index;
    if (node == NULL) {
        return ;
    }

    printf("node %p %s has children %d :\n",
           node, node->key == NULL? "NULL" : node->key, node->num);

    for(index = node->children; index != NULL;
        index = index->next_sibling) {
        rt_node_dump(index);
    }
}

void rt_dump(rt_t *tree)
{
    rt_node_dump(tree->root);
}

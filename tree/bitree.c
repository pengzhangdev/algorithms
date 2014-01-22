/* bitree.c --- binary tree
 *
 * Filename: bitree.c
 * Description: binary tree
 * Author: Werther Zhang (email: peng.zhang.dev@gmail.com)
 * Maintainer: Werther Zhang (email: peng.zhang.dev@gmail.com)
 * Created: Wed Jan 22 22:09:14 2014 (+0800)
 * Version: 0
 * Last-Updated: Wed Jan 22 22:21:33 2014 (+0800)
 *           By: Werther Zhang
 *     Update #: 1
 * URL:
 * Doc URL:
 * Keywords:
 *
 */

/* Change Log:
 * 2014/01/22 Werther
 *      complete the implementation
 *
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */


#include <stdlib.h>
#include <string.h>

#include "bitree.h"

void bitree_init(struct bitree_tree *tree, void (*destroy)(void *data))
{
    tree->size = 0;
    tree->destroy = destroy;
    tree->root = NULL;

    return;
}

void bitree_destroy(struct bitree_tree *tree)
{
    bitree_rm_left(tree, NULL);
    bzero(tree, sizeof(struct bitree_tree));
    return;
}

int bitree_node_init(struct bitree_node **node, const void *data)
{
    (*node) = (struct bitree_node *)malloc(sizeof(struct bitree_node));
    if (*node == NULL) {
        return -1;
    }

    (*node)->data = (void *)data;
    (*node)->left = NULL;
    (*node)->right = NULL;

    return 0;
}

int bitree_ins_left_data(struct bitree_tree *root,
                    struct bitree_node *parent, const void *data)
{
    struct bitree_node *new_node, **position;

    if (parent == NULL) {
        if (bitree_size(root) > 0)
            return -1;

        position = &root->root;
    } else {
        if (bitree_left(parent) != NULL)
            return -1;

        position = &parent->left;
    }

    if (bitree_node_init(&new_node, data) == -1)
        return -1;

    *position = new_node;

    root->size++;

    return 0;
}

int bitree_ins_right_data(struct bitree_tree *root,
                     struct bitree_node *parent, const void *data)
{
    struct bitree_node *new_node, **position;

    if (parent == NULL) {
        if (bitree_size(root) > 0)
            return -1;

        position = &root->root;
    } else {
        if (bitree_right(parent) != NULL)
            return -1;

        position = &parent->right;
    }

    if (bitree_node_init(&new_node, data))
        return -1;

    *position = new_node;

    root->size++;

    return 0;
}

int bitree_ins_left_node(struct bitree_tree *root,
                    struct bitree_node *parent, struct bitree_node *left)
{
    struct bitree_node **position;

    if (parent == NULL) {
        if (bitree_size(root) > 0)
            return -1;

        position = &root->root;
    } else {
        if (bitree_left(parent) != NULL)
            return -1;

        position = &parent->left;
    }

    *position = left;

    root->size++;

    return 0;
}

int bitree_ins_right_node(struct bitree_tree *root,
                     struct bitree_node *parent, struct bitree_node *right)
{
    struct bitree_node *new_node, **position;

    if (parent == NULL) {
        if (bitree_size(root) > 0)
            return -1;

        position = &root->root;
    } else {
        if (bitree_right(parent) != NULL)
            return -1;

        position = &parent->right;
    }

    *position = right;

    root->size++;

    return 0;
}

int bitree_merge(struct bitree_tree *merge, struct bitree_tree *left,
                 struct bitree_tree *right, const void *data)
{
    bitree_init(merge, left->destroy);

    if (bitree_ins_left_data(merge, NULL, data) != 0) {
        bitree_destroy(merge);
        return -1;
    }

    bitree_root(merge)->left = bitree_root(left);
    bitree_root(merge)->right = bitree_root(right);

    merge->size = merge->size + bitree_size(left) + bitree_size(right);

    left->root = NULL;
    left->size = 0;
    right->root = NULL;
    right->size = 0;

    return 0;
}

int bitree_split(struct bitree_tree *tree, struct bitree_tree *left,
                 struct bitree_tree *right)
{
    bitree_init(left, tree->destroy);
    bitree_init(right, tree->destroy);

    left->root = bitree_root(tree)->left;
    right->root = bitree_root(tree)->right;

    // we will free it at root
    //bitree_root(tree)->left = NULL;
    //bitree_root(tree)->right = NULL;
    //tree->size = 0;

    left->size = bitree_node_count(bitree_root(left));
    right->size = bitree_node_count(bitree_root(right));
    return 0;
}

int bitree_node_count(struct bitree_node *node)
{
    if (node == NULL)
        return 0;

    int count = 0;
    if (node->left != NULL) {
        count += bitree_node_count(node->left);
    }

    if (node->right != NULL) {
        count += bitree_node_count(node->right);
    }

    count++;

    return count;
}

void bitree_rm_left(struct bitree_tree *tree, struct bitree_node *node)
{
    struct bitree_node **position;

    if (bitree_size(tree) == 0) {
        return;
    }

    if (node == NULL) { //rm all tree
        position = &tree->root;
    } else {
        position = &node->left;
    }

    // remove the node
    if (*position != NULL) {
        bitree_rm_left(tree, *position);
        bitree_rm_right(tree, *position);

        if (tree->destroy != NULL) {
            tree->destroy((*position)->data);
        }

        free(*position);
        *position = NULL;

        tree->size--;
    }

    return;
}

void bitree_rm_right(struct bitree_tree *tree, struct bitree_node *node)
{
    struct bitree_node **position;

    if (bitree_size(tree) == 0) {
        return ;
    }

    if (node == NULL) {
        position = &tree->root;
    } else {
        position = &node->right;
    }

    if (*position != NULL)
    {
        bitree_rm_left(tree, *position);
        bitree_rm_right(tree, *position);

        if (tree->destroy != NULL) {
            tree->destroy((*position)->data);
        }

        free(*position);
        *position = NULL;

        tree->size--;
    }
    return;
}

void show_tree_preorder(struct bitree_tree *tree, struct bitree_node *node,
                        void (*show_data)(void *data))
{
    struct bitree_node *current;
    if (tree != NULL) {
        current = bitree_root(tree);
    } else if (node != NULL) {
        current = node;
    } else {
        return;
    }

    show_data(current->data);
    show_tree_preorder(NULL, current->left, show_data);
    show_tree_preorder(NULL, current->right, show_data);
}
void show_tree_posorder(struct bitree_tree *tree, struct bitree_node *node,
                        void (*show_data)(void *data))
{
    struct bitree_node *current;
    if (tree != NULL) {
        current = bitree_root(tree);
    } else if (node != NULL) {
        current = node;
    } else {
        return;
    }

    show_tree_posorder(NULL, current->left, show_data);
    show_tree_posorder(NULL, current->right, show_data);
    show_data(current->data);
}
void show_tree_inorder(struct bitree_tree *tree, struct bitree_node *node,
                       void (*show_data)(void *data))
{
    struct bitree_node *current;
    if (tree != NULL) {
        current = bitree_root(tree);
    } else if (node != NULL) {
        current = node;
    } else {
        return ;
    }

    show_tree_inorder(NULL, current->left, show_data);
    show_data(current->data);
    show_tree_inorder(NULL, current->right, show_data);
}

void build_tree_data(struct bitree_tree *tree, struct bitree_node *node ,
                     void (*build_data)(struct bitree_node *node, char *value), char *value)
{
    struct bitree_node *current;
    if (tree != NULL) {
        current = bitree_root(tree);
    } else if (node != NULL) {
        current = node;
    } else {
        return ;
    }

    build_data(current, value);
    //build_tree_data(NULL, current, build_data, data);
    //build_data(current, value);
    //build_tree_data(NULL, current, build_data, data);
}

/* bitree.c ends here */

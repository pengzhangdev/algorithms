#ifndef BIT_TREE_H
#define BIT_TREE_H

struct bitree_node {
    void *data;
    struct bitree_node *left;
    struct bitree_node *right;
};

struct bitree_tree {
    int size;
    struct bitree_node *root;
    int (*compare)(const void *key1, const void *key2);
    void (*destroy)(void *data); // destroy data;
};

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * init the bit tree.
     * @tree, to be inited.
     * @destroy, the destroy function for data.
     */
    void bitree_init(struct bitree_tree *tree, void (*destroy)(void *data));

    /**
     * destroy bitree, include all data.
     */
    void bitree_destroy(struct bitree_tree *tree);


    /**
     * init bitree_node
     */

    int bitree_node_init(struct bitree_node **node, const void *data);

    /**
     * init bitree_node for @data and insert it as the left child of @parent.
     * If @parent is NULL, create @root.
     */
    int bitree_ins_left_data(struct bitree_tree *root,
                             struct bitree_node *parent, const void *data);

    /**
     * init bitree_node for @data and insert it as the right child of @parent.
     * If @parent is NULL, create @root.
     */
    int bitree_ins_right_data(struct bitree_tree *root,
                              struct bitree_node *parent, const void *data);

    /**
     * insert @left as the left child of parent.
     * If @parent is NULL, create @root.
     */
    int bitree_ins_left_node(struct bitree_tree *root,
                             struct bitree_node *parent, struct bitree_node *left);

    /**
     * insert @right as the right child of parent.
     * If @parent is NULL, create @root.
     */
    int bitree_ins_right_node(struct bitree_tree *root,
                              struct bitree_node *parent, struct bitree_node *right);

    /**
     * split @root as two tree.
     */
    int bitree_split(struct bitree_tree *root, struct bitree_tree *left,
                     struct bitree_tree *right);


    int bitree_node_count(struct bitree_node *node);
    /**
     * merge two tree into @merge, and the data of @merge is @data.
     */
    int bitree_merge(struct bitree_tree *merge, struct bitree_tree *left,
                     struct bitree_tree *right, const void *data);

    void bitree_rm_left(struct bitree_tree *root, struct bitree_node *node);
    void bitree_rm_right(struct bitree_tree *root, struct bitree_node *node);

    void show_tree_preorder(struct bitree_tree *tree, struct bitree_node *node,
                            void (*show_data)(void *data));
    void show_tree_posorder(struct bitree_tree *tree, struct bitree_node *node,
                            void (*show_data)(void *data));
    void show_tree_inorder(struct bitree_tree *tree, struct bitree_node *node,
                           void (*show_data)(void *data));

    void build_tree_data(struct bitree_tree *tree, struct bitree_node *node ,
                         void (*build_data)(struct bitree_node *node, char *value), char *value);
#ifdef __cplusplus
}
#endif

#define bitree_size(tree) ((tree)->size)
#define bitree_root(tree) ((tree)->root)
#define bitree_is_leaf(node) ((node)->left == NULL && (node)->right == NULL)
#define bitree_data(node) ((node)->data)
#define bitree_left(node) ((node)->left)
#define bitree_right(node) ((node)->right)
#define bitree_is_eob(node) ((node) == NULL)
// #define data2node(data) \
//     (struct bitree_node*)((char *)data - \
//                           (unsigned long)(((struct bitree_node*)(0))->data))

#endif

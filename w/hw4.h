#ifndef _HW4_H_
#define _HW4_H_

struct ibst {
  int root;
  struct ibst *lsub;
  struct ibst *rsub;
};

struct ilist {
  int first;
  struct ilist *rest;
};

typedef struct ibst  ibst_t;
typedef struct ilist ilist_t;

// build a bst containing the numbers in array
// - the bst must not contain any duplicates
// - optional challenge: build a balanced bst (not lopsided)
ibst_t *build_bst(int arr[], int len);

// verify that the bst is well formed, that is, contains no numbers out of place
int well_formed(ibst_t *t);

// compute the sum of the bst *without recursion*
int sum(ibst_t *t);

// return the path to the sought number, root first
// if the sought number is absent, return NULL
ilist_t *path_to(int sought, ibst_t *t);

// return the preorder traversal of the tree
ilist_t *preorder(ibst_t *t);

// return the inorder traversal of the tree
ilist_t *inorder(ibst_t *t);

// return the postorder traversal of the tree
ilist_t *postorder(ibst_t *t);

// return the level-order traversal of the tree
ilist_t *levelorder(ibst_t *t);

// build a string representation of the tree
char *bst_tos(ibst_t *t);

#endif

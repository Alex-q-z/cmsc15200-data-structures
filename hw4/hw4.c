#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw4.h"

struct ibst_list{
  ibst_t *first;
  struct ibst_list *rest;
};

typedef struct ibst_list ibst_list_t;

// helper function of selection_sort
void pull_min(int arr[], int len){
  if(len <= 1) return ;
  int minidex = 0;
  int minval = arr[0];
  for(int i=1; i<len; i++){
    if(arr[i] < minval){
      minidex = i;
      minval = arr[i];
    }
  }
  int tmp = arr[0];
  arr[0] = arr[minidex];
  arr[minidex] = tmp;
}

// implement selction sort
void selection_sort(int arr[], int len){
  while(len >= 1){
    pull_min(arr++,len--);
  }
}

// remove duplicates from the given array and return the new array's length
int remove_duplicate(int arr[], int len){
  if(len == 0 || len == 1){
    return len;
  }
  int k = 0, tmp[len];
  int same = 0;
  for(int i=0; i<len; i++){
    for(int j=0; j<i; j++){
      if(arr[i] == arr[j]){
	same = 1;
	break;
      }
    }
    if(same == 0){
      tmp[k] = arr[i];
      k++;
    }
    same = 0;
  }
  for(int i=0; i<k; i++){
    arr[i] = tmp[i];
  }
  return k;
}

// build a bst containing the numbers in array
// - the bst must not contain any duplicates
// - optional challenge: build a balanced bst (not lopsided)
ibst_t *build_bst(int arr[], int len){
  if(len == 0){
    return NULL;
  }else{
    ibst_t *res = malloc(sizeof(ibst_t));
    len = remove_duplicate(arr,len);
    selection_sort(arr,len);
        
    int i=0, j=i+len-1;
    int mid=(i+j)/2; 
    res->root = arr[mid];
    res->lsub = build_bst(arr,mid);
    res->rsub = build_bst(arr+mid+1,len-mid-1);
    return res;
  }
}

// helper function used for testing
void print_tree(ibst_t *t){
  if(t != NULL){
    print_tree(t->lsub);
    printf("%d ",t->root);
    print_tree(t->rsub);
  }
}

// verify that the bst is well formed, 
// that is, contains no numbers out of place
int well_formed(ibst_t *t){
  if(t == NULL){
    return 1;
  }else{
    if(t->lsub == NULL && t->rsub == NULL)
      return 1;
    else if(t->lsub == NULL){
      if(t->root >= t->rsub->root)
	return 0;
      else 
	return well_formed(t->rsub);
    }
    else if(t->rsub == NULL){
      if(t->root <= t->lsub->root)
	return 0;
      else 
	return well_formed(t->lsub);
    }
    else{
      if(t->root <= t->lsub->root ||
	 t->root >= t->rsub->root){
	return 0;
      }else{
	return well_formed(t->lsub) * well_formed(t->rsub);
      }
    }
  }
}

ibst_list_t *push(ibst_t *f, ibst_list_t *r){
  if(f == NULL)
    return r;
  ibst_list_t *res = malloc(sizeof(ibst_list_t));
  res->first = f;
  res->rest = r;
  return res;
}

// compute the sum of the bst *without recursion*
int sum(ibst_t *t){
  int res = 0;
  ibst_list_t *stack = malloc(sizeof(ibst_list_t));
  stack->first = t;
  stack->rest = NULL;
  while(stack != NULL){
    ibst_t *top = stack->first;
    stack = stack->rest;
    if(top->lsub != NULL)
      stack = push(top->lsub,stack);
    if(top->rsub != NULL)
      stack = push(top->rsub,stack);
    res += top->root;
  }
  free(stack);
  return res;
}

// helper function used for testing
void print_list(ilist_t *t){
  if(t != NULL){
    printf("%d ",t->first);
    print_list(t->rest);
  }
}

// helper function
int ifpresent(int sought, ibst_t *t){
  if(t != NULL){
    if(t->root == sought)
      return 1;
    else if(t->root < sought && t->rsub != NULL)
      return ifpresent(sought,t->rsub);
    else if(t->root > sought && t->lsub != NULL)
      return ifpresent(sought,t->lsub);
    else 
      return 0;
  }
  return 0;
}

// return the path to the sought number, root first
// if the sought number is absent, return NULL
ilist_t *path_to(int sought, ibst_t *t){
  ilist_t *res = malloc(sizeof(ilist_t));
  if(ifpresent(sought,t) == 0)
    return NULL;
  if(t != NULL){
    if(t->root == sought){
      res->first = t->root;
      res->rest = NULL;
      return res;
    }
    else if(t->root < sought){
      res->first = t->root;
      res->rest = path_to(sought,t->rsub);
    }
    else{
      res->first = t->root;
      res->rest = path_to(sought,t->lsub);
    }
  }
  return res;
}

// append two lists
ilist_t *append(ilist_t *a, ilist_t *b){
  if(a == NULL)
    return b;
  ilist_t *res = malloc(sizeof(ilist_t));
  res = a;
  while(res->rest != NULL){
    res = res->rest;
  }
  res->rest = b;
  return a;
}

// return the preorder traversal of the tree
ilist_t *preorder(ibst_t *t){
  ilist_t *res = malloc(sizeof(ilist_t));
  if(t == NULL)
    return NULL;
  res->first = t->root;
  res->rest = preorder(t->lsub);
  ilist_t *rlist = preorder(t->rsub);
  return append(res,rlist);
}

// return the inorder traversal of the tree
ilist_t *inorder(ibst_t *t){
  ilist_t *res = malloc(sizeof(ilist_t));
  if(t == NULL)
    return NULL;
  ilist_t *ll = inorder(t->lsub);
  ilist_t *cl = malloc(sizeof(ilist_t));
  cl->first = t->root;
  cl->rest = NULL;
  res = append(ll,cl);
  free(cl);
  ilist_t *rl = inorder(t->rsub);
  return append(res,rl);
}

// return the postorder traversal of the tree
ilist_t *postorder(ibst_t *t){
  ilist_t *res = malloc(sizeof(ilist_t));
  if(t == NULL)
    return NULL;
  ilist_t *ll = postorder(t->lsub);
  ilist_t *rl = postorder(t->rsub);
  ilist_t *cl = malloc(sizeof(ilist_t));
  res = append(ll,rl);
  cl->first = t->root;
  cl->rest = NULL;
  return append(res,cl);
}

ilist_t *add_tail(int n, ilist_t *l){
  ilist_t *res = malloc(sizeof(ilist_t));
  res->first = n;
  res->rest = NULL;
  if(l == NULL)
    return res;
  return append(l,res);
}

ibst_list_t *enqueue(ibst_t *ins, ibst_list_t *q){
  ibst_list_t *res = malloc(sizeof(ibst_list_t));
  res->first = ins;
  res->rest = q;
  return res;
} 

ibst_t *dequeue(ibst_list_t **q_prime){
  ibst_list_t *q = *q_prime;
  if(q == NULL)
    return NULL;
  if(q->rest == NULL){
    ibst_t *temp = q->first;
    *q_prime = NULL;
    return temp;
  }
  ibst_list_t *curr = q;
  while(curr->rest->rest != NULL)
    curr = curr->rest;
  ibst_t *res = curr->rest->first;
  //curr->rest = NULL;
  free(curr->rest);
  curr->rest = NULL;
  return res;
}

void print_queue(ibst_list_t *in){
  if(in != NULL){
    printf("%d -> ",in->first->root);
    print_queue(in->rest);
  } else {
    printf("NULL\n");
  }
}

// return the level-order traversal of the tree
ilist_t *levelorder(ibst_t *t){
  ilist_t *res = malloc(sizeof(ilist_t));
  res = NULL;
  ibst_list_t *queue = malloc(sizeof(ilist_t));
  queue->first = t;
  queue->rest = NULL;
  while(queue != NULL){
    ibst_t *rear = dequeue(&queue);
    res = add_tail(rear->root,res);
    if(rear->lsub != NULL)
      queue = enqueue(rear->lsub,queue);
    if(rear->rsub != NULL)
      queue = enqueue(rear->rsub,queue);
  }
  return res;
}

// build a string representation of the tree
char *bst_tos(ibst_t *t){
  char buffer[128] = {0};
  char *res = buffer;
  if(t == NULL)
    res += snprintf(res,128,"{NULL}");
  else{
    res += snprintf(res,128,"{root: ");
    res += snprintf(res,128,"%d ",t->root);
    if(t->lsub != NULL)
      res += snprintf(res,128,"lsub: %s",bst_tos(t->lsub));
    if(t->rsub != NULL)
      res += snprintf(res,128,"rsub: %s",bst_tos(t->rsub));
    res += snprintf(res,128,"} ");
  }
  return strdup(buffer);
}

int main(){
  // test of build_bst
  int arr1[8] = {3,2,5,4,5,1,2,3};
  ibst_t *test = build_bst(arr1,8);
  print_tree(test);  

  // test of well_formed
  ibst_t *test2 = malloc(sizeof(ibst_t));
  ibst_t *test_l = malloc(sizeof(ibst_t));  
  test_l->root = 12;
  test_l->lsub = NULL;
  test_l->rsub = NULL;
  test2->root = 10;
  test2->lsub = test_l;
  test2->rsub = NULL;
  printf("%d\n",well_formed(test2));
  free(test_l);
  free(test2);
  ibst_t *test3 = malloc(sizeof(ibst_t));
  ibst_t *test_l2 = malloc(sizeof(ibst_t));
  test_l2->root = 12;
  test_l2->lsub = NULL;
  test_l2->rsub = NULL;
  test3->root = 15;
  test3->lsub = test_l2;
  test3->rsub = NULL;
  printf("%d\n",well_formed(test3));
  free(test_l2);
  free(test3);

  // test of sum
  ibst_t *sum0 = malloc(sizeof(ibst_t));
  ibst_t *suml = malloc(sizeof(ibst_t));
  ibst_t *sumr = malloc(sizeof(ibst_t));
  suml->root = 8;
  suml->lsub = NULL;
  suml->rsub = NULL;
  sumr->root = 12;
  sumr->lsub = NULL;
  sumr->rsub = NULL;
  sum0->root = 10;
  sum0->lsub = suml;
  sum0->rsub = sumr;
  printf("%d\n",sum(sum0));
  free(suml);
  free(sumr);
  free(sum0);

  // test of print_list
  ilist_t *t3 = malloc(sizeof(ilist_t));
  ilist_t *t2 = malloc(sizeof(ilist_t));
  ilist_t *t1 = malloc(sizeof(ilist_t));
  t3->first = 12;  
  t3->rest = NULL;  
  t2->first = 8;
  t2->rest = t3;
  t1->first = 15;
  t1->rest = t2;
  print_list(t1);
  free(t3);
  free(t2);
  free(t1);

  // test of path_to
  ibst_t *test4 = malloc(sizeof(ibst_t));                                     
  ibst_t *test_l4 = malloc(sizeof(ibst_t));                                   
  ibst_t *test_l5 = malloc(sizeof(ibst_t));
  test_l5->root = 14;
  test_l5->lsub = NULL;
  test_l5->rsub = NULL;
  test_l4->root = 12;                                                         
  test_l4->lsub = NULL;                                                        
  test_l4->rsub = test_l5;                                                    
  test4->root = 15;                                                           
  test4->lsub = test_l4;                                                       
  test4->rsub = NULL;                                                         
  ilist_t *res1 = path_to(14,test4);
  print_list(res1);
  ilist_t *res2 = path_to(13,test4);                                          
  print_list(res2);
  free(res1);
  free(res2);
  free(test_l5);
  free(test_l4);                                                              
  free(test4);
  
  // test of preorder, inorder, postorder, levelorder
  ibst_t *test5 = malloc(sizeof(ibst_t));
  ibst_t *test_l6 = malloc(sizeof(ibst_t));
  ibst_t *test_l7 = malloc(sizeof(ibst_t));
  test_l7->root = 12;
  test_l7->lsub = NULL;
  test_l7->rsub = NULL;
  test_l6->root = 8;
  test_l6->lsub = NULL;
  test_l6->rsub = NULL;
  test5->root = 10;
  test5->lsub = test_l6;
  test5->rsub = test_l7;
  ilist_t *res_pre = preorder(test5);
  print_list(res_pre);
  free(res_pre);
  ilist_t *res_in = inorder(test5);
  print_list(res_in);
  free(res_in);
  ilist_t *res_post = postorder(test5);
  print_list(res_post);
  free(res_post);
  ilist_t *res_level = levelorder(test5);
  print_list(res_level);
  free(res_level);

  // test of bst_tos
  ibst_t *test6 = malloc(sizeof(ibst_t));
  test6 = NULL;
  printf("%s\n",bst_tos(test6)); 
  printf("%s\n",bst_tos(test5));
  free(test_l7);                                                              
  free(test_l6);                                                              
  free(test5);

  return 0;
}

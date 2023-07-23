#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool started = false;
static Code c;

Node *build_tree(uint64_t hist[static ALPHABET]) {
  PriorityQueue *q = pq_create(ALPHABET);
  for (uint32_t i = 0; i < ALPHABET; i++) {
    if (hist[i] > 0) {
      enqueue(q, node_create(i, hist[i]));
    }
  }
  while (pq_size(q) > 1) {
    Node *left = NULL;
    Node *right = NULL;
    Node *parent = NULL;
    dequeue(q, &left);
    dequeue(q, &right);
    parent = node_join(left, right);
    enqueue(q, parent);
  }

  Node *root = NULL;
  dequeue(q, &root);
  pq_delete(&q);

  return root;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
  uint8_t garbit;

  if (started == false) {
    Code d = code_init();
    c = d;
    started = true;
  }

  if (root != NULL) {
    if (root->left == NULL && root->right == NULL) {
      table[root->symbol] = c;
    } else {
      code_push_bit(&c, 0);
      build_codes(root->left, table);
      code_pop_bit(&c, &garbit);

      code_push_bit(&c, 1);
      build_codes(root->right, table);
      code_pop_bit(&c, &garbit);
    }
  }
  return;
}

void dump_tree(int outfile, Node *root) {
  if (root != NULL) {
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);

    if (root->left == NULL && root->right == NULL) {
      uint8_t L = 'L';
      write_bytes(outfile, &L, 1); // write binary char to file
      uint8_t s = root->symbol;
      write_bytes(outfile, &s, 1); // write symbol
    } else {
      uint8_t I = 'I';
      write_bytes(outfile, &I, 1);
    }
  }
}

Node *rebuild_tree(
    uint16_t nbytes,
    uint8_t tree[static nbytes]) { // tree_size is nbytesis how big the dump is
  Stack *s = stack_create(nbytes + 1); // stack can be of size at most nbytes + 1 at any given time 
  for (uint16_t i = 0; i < nbytes; i++) { // 
    if (tree[i] == 'L') { // if tree[i] = L then tree[i + 1] will be a symbol
      Node *n = node_create(tree[i + 1], 0);
      stack_push(s, n);
      i++;

    } else if (tree[i] == 'I') {
      Node *right;
      Node *left;
      stack_pop(s, &right);
      stack_pop(s, &left);
      Node *parent = node_join(left, right);
      stack_push(s, parent);
    }
  }
  Node *root;
  stack_pop(s, &root);

  return root;
}

void delete_tree(Node **root) {
  if ((*root) != NULL) {
    delete_tree(&((*root)->left));  // uses post-order traversal to delete all children first
    delete_tree(&((*root)->right));
    node_delete(root);
  }
  (*root) = NULL;
  // do nothing
}

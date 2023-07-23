#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*
typedef struct Node Node;

struct Node {
    Node *left;
    Node *right;
    uint8_t symbol;
    uint64_t frequency;
};
*/

Node *node_create(uint8_t symbol, uint64_t frequency) {
  Node *n = (Node *)malloc(sizeof(Node));
  if (n) {
    n->symbol = symbol;
    n->frequency = frequency;
    //	n->left = NULL; // unnecsarry
    //	n->right = NULL;
  }
  return n;
}

void node_delete(Node **n) {
  if (*n) {
    free(*n);
  }

  (*n) = NULL;
}

Node *node_join(Node *left, Node *right) {
  uint64_t sum = (left->frequency) + (right->frequency);
  Node *n = node_create('$', sum); // creates interior node
  n->left = left;
  n->right = right;
  return n;
}

void node_print(Node *n) {
  if (isprint(n->symbol) && !iscntrl(n->symbol)) {
    printf("n->symbol = %c\n", (char)n->symbol); 
  } else {
    printf("n->symbol = 0x%02" PRIx8 "\n", n->symbol);
  }
  printf("n->frequency = %lu\n", n->frequency);
  return;
}

bool node_cmp(Node *n, Node *m) {
  return n->frequency > m->frequency ? true : false;
}

void node_print_sym(Node *n) {
  if (isprint(n->symbol) && !iscntrl(n->symbol)) {
    printf("n->symbol = %c\n", (char)n->symbol);
  } else {
    printf("n->symbol = 0x%02" PRIx8 "\n", n->symbol);
  }
  return;
}

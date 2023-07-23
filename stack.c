#include "stack.h"
#include "node.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Stack Stack;

struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
};

Stack *stack_create(uint32_t capacity) {
  Stack *s = (Stack *)malloc(sizeof(Stack));
  if (s) {
    s->top = 0;
    s->capacity = capacity;
    s->items = (Node **)calloc(capacity, sizeof(Node *)); // array of node == pointer to pointer
  }
  return s;
}

void stack_delete(Stack **s) {
  for (uint32_t i = 0; i < stack_size(*s); i++) {
    if (*(((*s)->items) + i) != NULL) { // deletes each item off the stack
      node_delete(&(*(((*s)->items) + i)));
    }
  }
  *s = NULL;
}

bool stack_empty(Stack *s) {
  if (stack_size(s) == 0) {
    return true;
  }
  return false;
}

bool stack_full(Stack *s) {
  if (stack_size(s) == s->capacity) {
    return true;
  }
  return false;
}

uint32_t stack_size(Stack *s) { return s->top; }

bool stack_push(Stack *s, Node *n) {
  if (stack_full(s)) {
    return false;
  }

  *((s->items) + s->top) = n; // if not full append node
  s->top += 1;
  return true;
}

bool stack_pop(Stack *s, Node **n) {
  if (!stack_empty(s)) { // if stack not empty
    *n = *((s->items) + s->top - 1); // get the top item 
    *((s->items) + s->top) = NULL; // set that pointer to null
    s->top -= 1;
    return true;
  } else {
    return false;
  }
}

void stack_print(Stack *s) {
  for (uint32_t i = 0; i < stack_size(s); i++) {
    node_print(*((s->items) + i));
  }
  return;
}

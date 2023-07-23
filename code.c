#include "code.h"
#include "defines.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/*
typedef struct {
    uint32_t top;
    uint8_t bits[MAX_CODE_SIZE];
} Code;
*/

Code code_init(void) {
  Code c;
  c.top = 0;
  for (uint32_t i = 0; i < MAX_CODE_SIZE; i++) {  // zero out to remove junk
    c.bits[i] = 0;
  }
  return c;
}

uint32_t code_size(Code *c) { return c->top; }  

bool code_empty(Code *c) {  
  if (code_size(c) == 0) {
    return true;
  } else {
    return false;
  }
}

bool code_full(Code *c) {
  if (code_size(c) == ALPHABET) {
    return true;
  } else {
    return false;
  }
}

bool code_set_bit(Code *c, uint32_t i) {
  uint32_t chunk = i / 8;  // returns index
  uint32_t index = i % 8; // returns bit index
  if (index + chunk > 255) {
    return false;
  }
  uint8_t one = 1;
  c->bits[chunk] = c->bits[chunk] | one << index;
  return true;
}

bool code_clr_bit(Code *c, uint32_t i) {
  uint32_t chunk = i / 8; // index
  uint32_t index = i % 8; // bit index
  if (index + chunk > 255) {
    return false;
  }
  uint8_t one = 1;
  c->bits[chunk] = c->bits[chunk] & ~(one << index); // 0000.1000 -> 1111.0111
  return true;
}

bool code_get_bit(Code *c, uint32_t i) {
  uint32_t chunk = i / 8;
  uint32_t index = i % 8;
  uint8_t one = 1;
  if (index + chunk > 255) {
    return false;
  }
  uint8_t ans = c->bits[chunk] & one << index; // masks to get bit
  return ans;
}

bool code_push_bit(Code *c, uint8_t bit) {
  if (code_full(c)) {
    return false;
  }
  uint32_t chunk = code_size(c) / 8;
  uint32_t index = code_size(c) % 8;
  c->bits[chunk] = c->bits[chunk] | bit << index;
  c->top += 1;
  return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) {
  if (code_empty(c)) {
    return false;
  }
  c->top -= 1;
  uint32_t index = code_size(c);
  uint8_t ans = code_get_bit(c, index);
  *bit = ans;
  if (bit == NULL) {
    fprintf(stderr, "Code.c: [ERROR] bit pointer is NULL.\n");
  }
  code_clr_bit(c, index); // clears bit after popping/unncessary from the perspetive of the interface but cleaner
  return true;
}

void code_print(Code *c) {
  for (uint32_t i = 0; i < code_size(c); i++) {
    if (i % 64 == 0) {
      printf("\n");
    }
    printf("%hu", code_get_bit(c, i));
  }
  printf("\n");
  return;
}

#include "io.h"
#include "code.h"
#include "string.h"
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;
static uint8_t buffer[BLOCK];
static uint8_t wbuffer[BLOCK];
static uint32_t last_byte = 0;
static uint32_t offset = 0;
static uint32_t bits = 0;
static bool rfinished = false;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
  int i = 0;
  int j;

  while (nbytes - i > 0 && (j = read(infile, buf + i, nbytes - i)) >
                               0) { // if read n bytes read n less bytes
    i += j;
  }
  
  bytes_read += i;
  return i;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  int i = 0;
  int j;

  while (nbytes - i > 0 && (j = write(outfile, buf + i, nbytes - i)) >
                               0) { // if write n bytes then write n less bytes
    i += j;
    //	printf("i\n");
  }
  //	printf("j\n");
  bytes_written += i;
  return i;
}

bool read_bit(int infile, uint8_t *bit) {
  uint8_t byte = 1;
  if (rfinished) {
    return false;
  }
  if (offset >= BLOCK * 8 || offset == 0) { // if fresh or greater than max bits
    last_byte =
        read_bytes(infile, buffer, BLOCK); // bufdex cannot be more than 4096
    offset = 0;
    if (last_byte == 0) {
      return false;
    }
  }

  *bit = (bool)(buffer[offset / 8] & (byte << (offset % 8)));
  offset++;

  if (last_byte < 4096) { // if we are only reading up to test
    if (offset / 8 == last_byte) {
      rfinished = true;
    }
  }
  return true;
}

void write_code(int outfile, Code *c) {
  uint8_t bit = 0;
  uint8_t one = 1;

  if (code_empty(c)) {
    return;
  }
  uint32_t i = 0;
  // until i < c->top
  while (bits < 32768 && i < c->top) {
    bit = code_get_bit(c, i);
    if (bit == 1) {
      wbuffer[bits / 8] = wbuffer[bits / 8] | (bit << (bits % 8));
    } else {
      wbuffer[bits / 8] = wbuffer[bits / 8] &
                          ~((one << (bits % 8))); // 0001 0000 --> 1110 1111 &
    }
    i++;
    bits++;
  }

  if (bits == 32768) { // if either nothing more to pop or overflowed, check if
                       // overflowed. If so, write the block.
    write_bytes(outfile, wbuffer, 4096);
    // flush_codes(outfile);
    bits = 0;
  } else { // if no overflow, don't reset bits or do anything. This also means
           // code_pop_bit() is what failed, so it should fail below
    // printf("%u\n", wbuffer[bits - 1]);
    return;
  }

  bits = 0;
  while (i < c->top) {
    bit = code_get_bit(c, i);
    if (bit == 1) {
      wbuffer[bits / 8] = wbuffer[bits / 8] | (bit << (bits % 8));
    } else {
      wbuffer[bits / 8] = wbuffer[bits / 8] &
                          ~((one << (bits % 8))); // 0001 0000 --> 1110 1111 &
    }
    bits++;
    i++;
  }
  return;
}

void flush_codes(int outfile) {
  uint8_t one = 1;

  while (bits % 8 != 0) { // if need to flush case
    wbuffer[bits / 8] =
        wbuffer[bits / 8] & ~(one << ((bits % 8))); // this seems faulty
    bits++;
  }

  write_bytes(outfile, wbuffer, ((bits - 1) / 8) + 1);
  return;
}

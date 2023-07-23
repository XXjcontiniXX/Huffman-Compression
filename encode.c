#include "code.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define OPTIONS "vho:i:"

int main(int argc, char **argv) {
  char help[] =
      "SYNOPSIS\n  A Huffman encoder.\n  Compresses a file using the Huffman "
      "coding algorithm.\n\nUSAGE\n  ./encode [-h] [-i infile] [-o "
      "outfile]\n\nOPTIONS\n  -h             Program usage and help.\n  -v     "
      "        Print compression statistics.\n  -i infile      Input file to "
      "compress.\n  -o outfile     Output of compressed data.\n";
  uint8_t v = 0;
  int infile = 0;
  int outfile = 1;
  int opt = 0;
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    switch (opt) {
    case 'v':
      v = v | (1 << 0);
      break;
    case 'h':
      v = v | (1 << 1);
      break;
    case 'o':

      if ((outfile = open(optarg, O_WRONLY)) < 0) {
        outfile = open(optarg, O_WRONLY | O_CREAT);
      }

      break;
    case 'i':
      if ((infile = open(optarg, O_RDONLY)) < 0) {
        return 1;
      }

      break;
    default:
      fprintf(stderr, "%s", help);
      return 1;
      break;
    }
  }

  if (v > 1) {
    fprintf(stderr, "%s", help);
    return 0;
  }

  // Making a tmpfile before everything to read for the code emitting
  int tmpdes = open("tmpdes.txt", O_RDWR | O_CREAT);
  //

  // initialized & zeroes histogram
  uint64_t hist[ALPHABET];
  for (uint32_t i = 0; i < ALPHABET; i++) {
    hist[i] = 0;
  }
  //

  // reads one byte at a time from infile
  uint8_t character[1];
  uint64_t bytes = 0;
  while (read_bytes(infile, character, 1) !=
         0) { // while there are bytes in the array bufferof[char] should be
              // incremented.
    write_bytes(tmpdes, character, 1);
    hist[character[0]] += 1;
    bytes++;
    // printf("%c: %lu\n", character[0], hist[character[0]]);
  }

  //

  // if the first and second symbol are zero make it them one
  if (hist[0] == 0) {
    hist[0] = 1;
  }
  if (hist[1] == 0) {
    hist[1] = 1;
  }
  //

  // builds tree and returns root
  Node *root = build_tree(hist);
  //

  // builds the codes
  Code table[ALPHABET];
  build_codes(root, table);
  // code_print(&table[105]);
  //

  // constructs a header containing metadata essentially
  uint16_t tree_size = 0;
  for (uint32_t i = 0; i < ALPHABET; i++) {
    if (hist[i] > 0) {
      tree_size++;
    }
  }
  tree_size = (tree_size * 3) - 1;
  ///

  struct stat *file_stat; // allocate space for stat pointer
  file_stat = malloc(sizeof(struct stat));
  uint64_t file_size = 0;
  if (file_stat == NULL) {
    fprintf(stderr, "encode: [ERROR] malloc failed to allocate memeory for the "
                    "file stat.\n");
    return 1;
  }

  fstat(infile, file_stat); // put infile's metadata in file_stat
  if (infile != 0) {
    file_size = file_stat->st_size; // st_size contains the size in bytes
  } else {
    file_size = bytes;
  }

  // printf("file_size: %lu\n", file_size);
  uint16_t permissions = file_stat->st_mode;
  // printf("permissions %u\n", permissions);

  free(file_stat);
  file_stat = NULL;

  if (fchmod(outfile, permissions) < 0) { // set outfiles permissions
    fprintf(
        stderr,
        "encode: [ERROR] outfile's permissions setting failed...err code: %u\n",
        errno);
    return 1;
  }
  ///

  // union creation
  union {
    uint8_t bytes[sizeof(Header)];
    Header h;
  } hu;

  (hu.h).magic = MAGIC;
  (hu.h).permissions = permissions;
  (hu.h).tree_size = tree_size;
  (hu.h).file_size = file_size;

  write_bytes(outfile, hu.bytes,
              sizeof(hu)); // writes to outfile, the header casted to uint8_t
                           // for the entire size of header
  // dump the tree to outfile
  dump_tree(outfile, root);

  // character[] was inited earlier
  lseek(tmpdes, 0, SEEK_SET);

  // printf("bytes_written: %lu\n", bytes_written);
  bytes_written -= bytes;
  while (read_bytes(tmpdes, character, 1)) {
    //	printf("\n");
    //	printf("\n");
    //	printf("one char\n");

    Code d = table[character[0]];

    write_code(outfile, &d);
  }

  flush_codes(outfile); // flushhhhhh

  if (infile != 0) {
    close(infile);
  }

  if (outfile != 1) {
    close(outfile);
  }

  remove("tmpdes.txt");

  // then destroy tree
  delete_tree(&root);
  // bytes_written -= subtracters;
  double percentage = (1 - ((double)bytes_written / (double)bytes)) * 100;
  if (v == 1) {
    fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes);
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
    fprintf(stderr, "Space saving: %.2f%%\n", percentage);
  }
  return 0;
}

#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "node.h"
#include "pq.h"
#include "code.h"
#include "io.h"
#include "huffman.h"
#include "header.h"
#include "stack.h"
#include <errno.h>
#define OPTIONS "vho:i:"

int main(int argc, char **argv) {
        char help[] = "SYNOPSIS\n  A Huffman decoder.\n  Decompresses a file using the Huffman coding algorithm.\n\nUSAGE\n  ./decode [-h] [-i infile] [-o outfile]\n\nOPTIONS\n  -h             Program usage and help.\n  -v             Print compression statistics.\n  -i infile      Input file to decompress.\n  -o outfile     Output of decompressed data.\n";
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

                                infile = open(optarg, O_RDONLY);
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
	uint8_t bit;
	// going to read in bytes from infile into the header
	union {
                uint8_t bytes[sizeof(Header)];
                Header h;
        } hu;
	
	read_bytes(infile, hu.bytes, sizeof(Header));
	
	// checking the magic numbers
       	if (MAGIC != (hu.h).magic) {
		fprintf(stderr, "decode: [ERROR] MAGIC number is incorrect.\n");
		return 1;
	}
	
	if (fchmod(outfile, (hu.h).permissions) < 0) { // set outfiles permissions
                fprintf(stderr, "decode: [ERROR] outfile's permissions setting failed...err code: %u\n", errno);
                return 1;
        }			
	
	uint8_t tree[(hu.h).tree_size];

	read_bytes(infile, tree, (hu.h).tree_size);
	
	Node *root = rebuild_tree((hu.h).tree_size, tree);
	
	
	//uint8_t buffer[ (hu.h).file_size < 1024 ? (hu.h).file_size : 1024 ];
	uint64_t counter = 0;
	Node *curr = root;
	while (counter < (hu.h).file_size && read_bit(infile, &bit)) { /// might have to have read bit read from the lsb of a bit and then read the next byte from the lsb
		if (bit == 1) {
			curr = curr->right; // if bit == 1 and its children arent NULL its internal continue right
			if (curr->left == NULL && curr->right == NULL) {
				uint8_t *sym = &curr->symbol;
				write_bytes(outfile, sym, 1);
				curr = root;
				counter++;
			}
			
		}else{ // if bit == 0 
			curr = curr->left; // if bit == 1 and its children arent NULL its internal continue right
			if (curr->left == NULL && curr->right == NULL) {
				uint8_t *sym = &curr->symbol;
                                write_bytes(outfile, sym, 1);
				curr = root;
				counter++;
                        }
		}

	
	}

	if (infile != 0) {
                close(infile);
        }

        if (outfile != 1) {
                close(outfile);
        }	


			
	// Kenjiro Tsuda
	
	
	return 0;
}

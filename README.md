# A Huffman encoding and decoding algorithm  
  
## How to encode  

  Compresses a file using the Huffman coding algorithm.
  
USAGE  
  `./encode [-h] [-i infile] [-o outfile]`  
  
OPTIONS  
  `-h`             Program usage and help.  
  `-v`             Print compression statistics.  
  `-i` infile      Input file to compress.  
  `-o` outfile     Output of compressed data.  

## How to decode  
  
  Decompresses a file using the Huffman coding algorithm.  

USAGE  
  `./decode [-h] [-i infile] [-o outfile]`  

OPTIONS  
  `-h`             Program usage and help.  
  `-v`             Print compression statistics.  
  `-i` infile      Input file to decompress.  
  `-o` outfile     Output of decompressed data.  
  
## File descriptions 
1. encode.c: This file contains my implementation of the Huffman encoder.  
  
2. decode.c: This file contains my implementation of the Huffman decoder.  
  
3. defines.h: This file contains macro definitions used throughout the assignment.  
    
4. header.h: This file contains the struct definition for a file header.  
  
5. node.h: This file contains the node ADT interface.  
  
6. node.c: This file contains my implementation of the node ADT.  
  
7. pq.h: This file contains the priority queue ADT interface.  
  
8. pq.c: This file contains my implementation of the priority queue ADT.  
  
9. code.h: This file contains the code ADT interface.    
  
10. code.c: This file contains my implementation of the code ADT.  
  
11. io.h: This file contains the I/O module interface.  

12. io.c: This file contains my implementation of the I/O module.  
  
13. stack.h: This file contains the stack ADT interface.  
  
14. stack.c: This file contains your implementation of the stack ADT.  
  
### Final notes  
The decode function will not attempt to decode a file unless the correct identification number 0xBEEFBBAD is explicitly delclared within the first four bytes of the file to be decoded.  
#### Happy compressing!

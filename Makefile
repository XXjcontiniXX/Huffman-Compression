SOURCES  = $(wildcard *.c)
#OBJECTS  = $(SOURCES:%.c=%.o)

CC       = clang
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -Ofast

.PHONY: all clean spotless format

all: encode decode

#test: node.o test.o pq.o code.o io.o stack.o huffman.o # get rid of this jesus please
#	$(CC) -o $@ $^

encode: pq.o node.o code.o stack.o io.o huffman.o encode.o
	$(CC) -o $@ $^

decode: pq.o node.o code.o stack.o io.o huffman.o decode.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o

spotless:
	rm -f encode decode *.{c, txt}

format:
	clang-format -i -style=file *.[ch]

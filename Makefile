CFLAGS=-Wall -pedantic -g
OFLAGS=-r -fPIC
SHLIBFLAGS=-rdynamic -shared -fPIC

my-malloc.so: my-malloc.o memblock.o btree.o list.o
		gcc $(SHLIBFLAGS) -o $@ $^

test: test.o btree.o list.o memblock.o my-malloc.so
	gcc -o $@ test.o btree.o list.o memblock.o

%.o: %.c %.h debug.h
	gcc $(CFLAGS) $(OFLAGS) -c -o $@ $*.c

.PHONY: clean
clean:
	rm -f *.o *.so test *.tmp

.PHONY: debug
debug: test
	gdb --args env LD_PRELOAD=./my-malloc.so ./test

.PHONY: tests
tests: test
	./tests.sh

.PHONY: ls
ls: my-malloc.so
	LD_PRELOAD=./my-malloc.so ls -l -R ~

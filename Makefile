CFLAGS=-Wall -pedantic -g -c
OFLAGS=-fPIC
SHLIBFLAGS=-rdynamic -shared -fPIC

nhmalloc.so: nhmalloc.o memblock.o btree.o list.o
		gcc $(SHLIBFLAGS) -o $@ $^

test: test.o btree.o list.o memblock.o nhmalloc.so
	gcc -o $@ test.o btree.o list.o memblock.o

%.o: %.c %.h debug.h
	gcc $(CFLAGS) $(OFLAGS) -o $@ $*.c

.PHONY: clean
clean:
	rm -f *.o *.so test *.tmp

.PHONY: debug
debug: test
	gdb --args env LD_PRELOAD=./nhmalloc.so ./test

.PHONY: tests
tests: test
	./tests.sh

.PHONY: ls
ls: nhmalloc.so
	LD_PRELOAD=./nhmalloc.so ls -l -R ~

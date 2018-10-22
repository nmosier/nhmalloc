CFLAGS=-Wall -pedantic -g
OFLAGS= -r -fPIC
SHLIBFLAGS=-rdynamic -shared -fPIC

my-malloc.so: my-malloc.o memblock.o btree.o list.o
		gcc $(SHLIBFLAGS) -o $@ $^

test: test.o btree.o list.o memblock.o my-malloc.so
	gcc -o $@ test.o btree.o list.o memblock.o

%.o: %.c
	gcc $(CFLAGS) $(OFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f *.o *.so test

.PHONY: debug
debug: test
	gdb --args env LD_PRELOAD=./my-malloc.so ./test

.PHONY: run
run: test
	./tests.sh

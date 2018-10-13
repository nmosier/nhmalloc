CFLAGS=-Wall -pedantic -g
OFLAGS= -r -fPIC
SHLIBFLAGS=-rdynamic -shared -fPIC

mymalloc.so: mymalloc.o bintree.o
		gcc $(SHLIBFLAGS) -o $@ $^

test: test.o bintree.o mymalloc.o
	gcc -o $@ $^

%.o: %.c
	gcc $(CFLAGS) $(OFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f *.o *.so mymalloc 

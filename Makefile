CFLAGS=-Wall -pedantic -g


mymalloc: mymalloc.o list.o
	gcc -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f *.o mymalloc

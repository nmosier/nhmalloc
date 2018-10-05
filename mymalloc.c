//Wriiten by Nick Mosier and Searidang Pa

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"


// NOTE: the list_node_t struct is now used instead
//keep information about each malloc allocation
/*
struct NODE_{
    size_t size_chunk;
    int free;  //4 bytes for an int?
    unsigned long long * p_start;
    unsigned long long  * p_end;
    struct NODE_ * next;
};
typedef struct NODE_ node;
*/

// typedefs
typedef list_t memblocks_t; // memblocks_t is the type of data structure being used to store headers for blocks of memory
typedef list_node_t memblock_t; // memblock_t is the actual header data type for each block of memory

//functions prototypes
void *mymalloc(size_t size);
void myfree(void *ptr);

// memblocks operations
void memblocks_init(list_t *memblocks);

// memblock operations
void memblock_insert(void *begin_addr, void *end_addr, list_t *memblocks);
list_node_t *memblock_find(size_t size, list_t *memblocks);
int memblock_split(list_node_t *block, size_t size, list_t *memblocks);
void memblock_free(void *begin_addr);
void memblock_allocate(void *begin_addr);

//node *find_available_spot(node *head, size_t size);
//node *push_node_end(node *head, size_t size, unsigned long long *p_used_end);
//node *find_end_node(node *head);


intptr_t BREAK_INCREMENT = 4096;
void *PROGRAM_BREAK_ADDR = NULL;
memblocks_t memblocks;

int main (int argc, char *argv[]){

   
   //small test

   char *strings[100];
   const char *base = "svjbv jkbdsvbksdvbdkfsvbfdkdksfsbjdfkdsjgbdsjkgbskvbsdv dskv sdkjV SDKV DKVBdkfbkdFABSJKDVASKDJVBSADKVBASDKVBSDKVBDSFKVBFDJKKJkkjbfkjfbvjkdsvsdkjvbdskjvbdskbdsv";
   for (int i = 0; i < 100; ++i) {
      strings[i] = mymalloc(i+1);
      char tmp[i+1];
      strncpy(tmp, base, i);
      tmp[i] = '\0';
      sprintf(strings[i], "%s", tmp);
   }

   for (int i = 0; i < 100; ++i) {
      printf("%3d: %s\n", i, strings[i]);
   }

   for (int i = 0; i < 100; ++i) {
      myfree(strings[i]);
   }

   char *strings2[100];
   for (int i = 0; i < 100; ++i) {
      strings2[i] = mymalloc(i+1);
      char tmp[i+1];
      strncpy(tmp, base, i);
      tmp[i] = '\0';
      sprintf(strings[i], "%s", tmp);
   }

   printf("checking pointer equality...\n");
   for (int i = 0; i < 100; ++i) {
      printf("%d: %s\n", i, (strings[i] == strings2[i]) ? "passed!" : "FAILED."); 
   }

   printf("memblocks:\n");
   list_print(&memblocks);

   //list_dump(stderr, &memblocks);
   printf("validating...%p\n", (void *) list_validate(&memblocks));

   
   exit(0);


   
   
   const char *msg = "This is a the first test of mymalloc.";
   for (int i = 0; i < 1000; ++i) {
      mymalloc(1000);
   }
   printf("list:\n");
   list_print(&memblocks);

   exit(0);


   printf("mallocating...");
   char *str = mymalloc(strlen(msg));
   sprintf(str, "%s", msg);
   printf("%p\n", (void *) str);

   printf("freeing...\n");
   myfree(str);
   list_print(&memblocks);

   exit(0);
   
   str = mymalloc(strlen(msg));
   printf("%p\n", (void *) str);

   list_print(&memblocks);
   
   str = mymalloc(strlen(msg));
   printf("%p\n", (void *) str);

   list_print(&memblocks);
   
   exit(0);
}


//to be implemented: where to store the nodes of the linked_list
// the nodes should be stored right before the addresses returned by malloc
void *mymalloc(size_t size) {
   memblock_t *memblock_header;
   void *memblock;

   /* check size is nonzero */
   if (size == 0) {
      return NULL;
   }

   /* initialize globals if necessary */
   if (PROGRAM_BREAK_ADDR == NULL) {
      /* intialize mymalloc globals */
      PROGRAM_BREAK_ADDR = sbrk(0);
      memblocks_init(&memblocks);
   }

   /* try to find suitable segment in list of memory blocks */
   do {
      memblock_header = memblock_find(size, &memblocks);
      if (memblock_header == NULL) {
         fprintf(stderr, "mymalloc: expanding the heap...\n");
         /* failed to find sufficiently sized memblock,
          * so reserve more */
         void *old_break, *new_break;
         old_break = sbrk(BREAK_INCREMENT);
         if (old_break == (void *) -1) {
            /* handle sbrk error */
            return NULL;
         }

         new_break = (void *) ((char *) old_break + BREAK_INCREMENT);
         memblock_insert(old_break, new_break, &memblocks);

         PROGRAM_BREAK_ADDR = new_break;
         BREAK_INCREMENT *= 2; // double size for next call to sbrk
      }
   } while (memblock_header == NULL);
   
   memblock_split(memblock_header, size, &memblocks);
   
   memblock = (void *) (memblock_header + 1);
   memblock_allocate(memblock);
   return memblock;
}

void myfree(void *ptr) {
   memblock_free(ptr);
}

/*
  //current breakpoint
   unsigned long long * brkp = sbrk(0);

  if (head == NULL){ //if the heap is empty
     printf("in");
     brk(brkp + Break_Increment_Size); //increase the breakpoint
     //create a node to store the meta_info
     //brkp + 1
  }
  else{
      //find available spot in the heap
      node * node_available = find_available_spot(head,size);
      node * end_node = find_end_node(head);
      unsigned long long * p_used_end = end_node -> p_end;  //find the end of the used memory

       //if there is an empty spot of a compatible size because there was a call to myfree()
       if (node_available != NULL){
           node_available -> free = 0;
           node_available -> size_chunk = size;
           return node_available -> p_start;
       }
       //else if there is enough memory to grow up and not pass the breakpoint
       else if (size + p_used_end < brkp){
          node * new_node = push_node_end(head, size, p_used_end);
          return new_node -> p_start;
       }

       //else there is not enough memory
       else if (size + p_used_end >= brkp){   //sbrk(0) returns the current breakpoint
          brk(brkp + Break_Increment_Size);     //set new the break point
          node * new_node = push_node_end(head, size, p_used_end);
          return new_node -> p_start;
        }
   }
   return NULL;
}

// an optimization would be to find the node with the min value such that its value is
// greater than or equal to the requested amount (minimize fragmentation)
node * find_available_spot(node * head, size_t size){
   node * current = head;
   while(current != NULL){
     if ((current -> size_chunk >= size) && current->free){
       return current;
     }
     current = current -> next;
   }
   return current;
}

node * push_node_end(node * head, size_t size, unsigned long long * p_used_end){
   //traverse through the linked list
   node * current = head;
   while(current != NULL){
     current = current -> next;
   }
   node * new_node = current -> next;
   new_node -> size_chunk = size;
   new_node -> p_end = p_used_end + size;
   new_node -> p_start = p_used_end + 4;  //is 4 the next address?
   new_node -> next = NULL;
   return new_node;
}

node * find_end_node(node * head){
   node * current = head;
   while(current != NULL){
      current = current -> next;
   }
   return current;
}

*/

void memblocks_init(list_t *memblocks) {
   memblocks->front = memblocks->back = NULL;
}

void memblock_insert(void *begin_addr, void *end_addr, list_t *memblocks) {
   list_node_t *memblock_header;
   
   /* initialize memblock header starting at begin_addr */
   memblock_header = (list_node_t *) begin_addr;
   memblock_header->size = (char *) end_addr - (char *) begin_addr - sizeof(list_node_t);
   memblock_header->free = true;

   /* append to memblocks list */
   list_append(memblock_header, memblocks);

   fprintf(stderr, "memblock_insert: begin_addr=%p, end_addr=%p, size=%zu\n", begin_addr, end_addr, memblock_header->size);
}

list_node_t *memblock_find(size_t size, list_t *memblocks) {
   return list_minlwrbnd(size, memblocks);
}

/* memblock_split
 * DESC: splits memory block into two different blocks, the first of size
 *       _size_ and the second of the remaining size
 * NOTE: will only split the memblock if both resulting blocks will be
 *       nonzero in size
 *
 */
int memblock_split(list_node_t *block, size_t size, list_t *memblocks) {
   size_t total_size = block->size;
   size_t remaining_size;
   list_node_t *block2;
   
   if (total_size <= size + sizeof(list_node_t)) {
      /* do not split blocks */
      return false;
   }

   /* split blocks */
   remaining_size = total_size - size - sizeof(list_node_t);
   block->size = size; // update size of first block
   block2 = (list_node_t *) ((char *) (block + 1) + size);

   /* initialize new block */
   block2->size = remaining_size;
   block2->free = block->free;

   /* insert into memblocks list */
   list_insertafter(block2, block, memblocks);

   return true;
}

void memblock_free(void *begin_addr) {
   list_node_t *block = ((list_node_t *) begin_addr) - 1;
   block->free = true;
}

void memblock_allocate(void *begin_addr) {
   list_node_t *block = ((list_node_t *) begin_addr) - 1;
   block->free = false;
}

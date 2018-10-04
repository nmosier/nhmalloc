//Wriiten by Nick Mosier and Searidang Pa

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
void * mymalloc(size_t size);
void * find_available_spot();

#define Break_Increment_Size 100

//keep information about each malloc allocation
struct node_{
    size_t size_chunk;
    int free? = 0;  //4 bytes for an int?
    void * p_start;
    void * p_end;
    struct node * next;
}
typedef struct node_ node;

node * head; //to be fixed since global variable is bad

int main (int argc, char* argv[]){
   //init head node to store meta info


   //small test
   char * s = mymalloc(5);
   *s = 'h';
   *(s+1) = 'e';
   *(s+2) = 'l';
   *(s+3) = 'l';
   *(s+4) = 'o';
   printf("%s \n", s);

}
//to be implemented: where to store the nodes of the linked_list
 void * mymalloc (size_t size){
  //find available spot in the heap
  node * node_available = find_available_spot(head,size);

  node * end_node = find_end_node(node * head);
  void * p_used_end = end_node -> p_end;  //find the end of the used memory

   //if there is an empty spot of a compatible size because there was a call to myfree()

   if (node_available != NULL){
       node_available -> free? = 0;
       node_available -> size_chunk=size;
       return node_available -> p_start;
   }
   //else if there is enough memory to grow up and not pass the breakpoint
   else if (size + p_used_end < sbrk(0)){
      node * new_node = push_node_end(head, size, p_used_end)
      return new_node -> p_start;
   }

   //else there is not enough memory
   else if (size + p_used_end >= sbrk(0)){   //sbrk(0) returns the current breakpoint
      brk(sbrk(0)+Break_Increment_Size);     //set new the break point
      node * new_node = push_node_end(head, size, p_used_end)
      return new_node -> p_start;
    }
}

node * find_available_spot(node * head, size_t size){
  node * current = head;
  while(current != NULL){
    if ((current -> size_chunk >= size) && current->free?){
      return current;
    }
    current = current -> next;
  }
  return current;
}

node * push_node_end(node * head, size_t size, void * p_used_end){
  node * new_node;
  new_node -> size_chunk = size;
  new_node -> p_end = p_used_end + size;
  new_node -> p_start = p_used_end + 4;  //is 4 the next address?

  //traverse through the linked list
  node * current = head;
  while(current != NULL){
    current = current -> next;
  }
  current -> next = new_node;
  current -> next -> next = NULL;
  return new_node
}

node * find_end_node(node * head){
  node * current = head;
  while(current != NULL){
     current = current -> next;
  }
  return current;
}

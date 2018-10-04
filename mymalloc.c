//Wriiten by Nick Mosier and Searidang Pa

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<Linked_List.c>
void * mymalloc(size_t size);
void * find_available_spot();

#define Break_Increment_Size 100

//keep information about each malloc allocation
struct node_{
    size_t size_chunk;
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

   //if there is an empty spot of a compatible size
   //because there was a call to myfree()
   void * p_available;   //find available spot in the heap


   //else if there is enough memory to grow up and not pass the breakpoint
   node * end_node = find_end_node(node * head);
   void * p_used_end = end_node -> p_end;  //find the end of the used memory


   //else there is not enough memory
   else if (size + p_used_end >= sbrk(0)){   //sbrk(0) returns the current breakpoint
      brk(sbrk(0)+Break_Increment_Size);     //set new the break point
      node * new_node;
      new_node -> size_chunk = size;
      new_node -> p_end = p_used_end + size;
      new_node -> p_start = p_used_end + 4;  //is 4 the next address?
      push_node_end(head, new_node)
      return new_node -> p_start;
    }
}

void * find_available_spot(size_t size){

}

void push_node_end(node * head, node * new_node){
  node * current = head;
  while(current != NULL){
    current = current -> next;
  }
  current -> next = new_node;
  current -> next -> next = NULL;
}

node*  find_end_node(node * head){
  node * current = head;
  while(current != NULL){
    current = current -> next;
  }
  return current;
}

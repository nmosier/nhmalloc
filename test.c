#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "memblock.h"

#define LOG(str) write(STDERR_FILENO, str, strlen(str))
char eprintf_buf[1000];
#define eprintf(fmt, ...) sprintf(eprintf_buf, fmt, __VA_ARGS__), LOG(eprintf_buf)

#define COLOR_RED   "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

#define SEED  100

int main(int argc, char *argv[]) {
   const int iterations = 3;
   const char *base = "assfdghjggbfvdcfvgbfvdcfgbhnbgfvcdfgbbsrcgbvfcdvbhgtrdcvbhtrdvbhgtrdcfvgbhgtfdxcvbhgfdcvbnhgfvbhgfcvbhgfcvbnhgfcvbnjhgrdesrtyuytrewert";
   const size_t malloc_max = strlen(base);
   char *strs[100];
   size_t strlens[100];


   void *ptrs[iterations];
   for (int i = 0; i < iterations; ++i) {
      ptrs[i] = malloc(i+1);
   }
   for (int i = 0; i < iterations; ++i) {
      free(ptrs[i]);
      //            memblocks_print(&memblocks);
   }

   return 0;

   // srandom(time(0));
   srandom(SEED);

   for (int i = 0; i < iterations; ++i) {
      strlens[i] = (random() % malloc_max) + 1;
      strs[i] = malloc(strlens[i]);
      sprintf(strs[i], "%.*s", (int) strlens[i] - 1, base);
   }

   for (int i = 0; i < iterations; ++i) {
      eprintf("%s\n", strs[i]);
      free(strs[i]);
      //      btree_print(memblocks);
   }

   //   memblocks_validate(&memblocks);
   exit(0);
   //   srandom(SEED);
   for (int i = 0; i < iterations; ++i) {
      strlens[i] = (random() % malloc_max);
      strs[i] = malloc(strlens[i]);
      sprintf(strs[i], "%.*s", (int) strlens[i] - 1, base);
      //      btree_print(memblocks);
   }

   return 0;
}

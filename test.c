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

   srandom(time(0));

   for (int i = 0; i < iterations; ++i) {
      strlens[i] = (random() % malloc_max) + 1;
      strs[i] = malloc(strlens[i]);
      sprintf(strs[i], "%.*s", (int) strlens[i] - 1, base);
   }

   for (int i = 0; i < iterations; ++i) {
      // compare strings
      if (strncmp(strs[i], base, strlens[i] - 1) != 0) {
         // print out correct vs. incorrect string
         eprintf("test: string mismatch at pointer %p:\n", (void *) strs[i]);
         eprintf(COLOR_RED"%s"COLOR_RESET, strs[i]);
         eprintf(COLOR_GREEN"%.*s"COLOR_RESET, (int) strlens[i], base);
      }
      
      free(strs[i]);
   }

   exit(0);

}

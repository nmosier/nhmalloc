#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include "memblock.h"
#include "debug.h"


//#define SEED  100
#define SEED time(NULL)
#define NPTRS 1000
#define NOPS 20000
#define MAXSIZE 4096

typedef struct {
   char *ptr;
   int filedes;
   off_t offset;
   size_t size;
} mirrored_ptr_t;


bool validate(mirrored_ptr_t *mirptr);
void read_mirror(mirrored_ptr_t *dst, int src_fd);

int main(int argc, char *argv[]) {
   /* get command line options */
   const char *optstring = "p:o:n:s:h";
   bool valid = true;
   int opt;
   int nptrs = NPTRS, nops = NOPS;
   unsigned int seed = SEED;
   size_t maxsize = MAXSIZE;

   while ((opt = getopt(argc, argv, optstring)) >= 0) {
      switch (opt) {
      case 'p':
         if (sscanf(optarg, "%d", &nptrs) < 0) {
            eprintf("%s: option -p (# of pointers): invalid integer value %s\n", argv[0], optarg);
            valid = false;
         }
         break;
      case 'o':
         if (sscanf(optarg, "%d", &nops) < 0) {
            eprintf("%s: option -o (# of malloc operations): invalid integer value %s\n",
                    argv[0], optarg);
            valid = false;
         }
         break;
      case 'n':
         if (sscanf(optarg, "%zu", &maxsize) < 0) {
            eprintf("%s: option -p: invalid integer value %s\n", argv[0], optarg);
            valid = false;
         }
         break;
      case 's':
         if (sscanf(optarg, "%ud", &seed) < 0) {
            eprintf("%s: option -p: invalid integer value %s\n", argv[0], optarg);
            valid = false;
         }
         break;
      case 'h':
      default:
         valid = false;
         break;
      }
   }

   if (!valid) {
      eprintf("usage: %s [-n maxsize] [-p nptrs] [-o noperations] [-s seed]\n", argv[0]);
      exit(3);
   }
   
   const char *tmp_file = "__test.tmp";
   int cmp_fd, urand_fd;
   mirrored_ptr_t ptrs[nptrs];
   int nsuccess = 0, nfail = 0;

   /* intialize ptrs array */
   for (int i = 0; i < nptrs; ++i) {
      mirrored_ptr_t ptr = {0};
      ptrs[i] = ptr;
   }

   
   // randomly select pointer from list
   // if nll, allocate a random size & fill
   // if not null, then free, compare mem, then write

   /* open comparison file */
   if ((cmp_fd = open(tmp_file, O_CREAT | O_TRUNC | O_RDWR)) < 0) {
      char sbuf[1000];
      sprintf(sbuf, "open: %s", tmp_file);
      perror(sbuf);
      exit(1);
   }

   /* open urandom file */
   if ((urand_fd = open("/dev/urandom", O_RDONLY)) < 0) {
      perror("open: /dev/urandom");
      exit(1);
   }
   
   srandom(seed);

   /* MAIN TEST LOOP */
   for (int op = 0; op < nops; ++op) {
      int index = random() % nptrs;
      if (ptrs[index].ptr) {
         /* validate memory contents then free */
         if (validate(&ptrs[index])) {
            ++nsuccess;
         } else {
            eprintf("test: memory validation failed for index %d (%p)\n", index, (void *) &ptrs[index]);
            ++nfail;
         }
         free(ptrs[index].ptr);

         mirrored_ptr_t ptr = {0};
         ptrs[index] = ptr;
      } else {
         int size;

         /* allocate random size */
         size = random() % maxsize;
         ptrs[index].size = size;
         ptrs[index].ptr = (char *) malloc(size);
         
         if (ptrs[index].ptr == NULL && ptrs[index].size > 0) {
            perror("test: null ptr returned by malloc");
            exit(1);
         }

         ptrs[index].filedes = cmp_fd;
         
         /* copy random bytes to pointer mem location */
         read_mirror(&ptrs[index], urand_fd);
      }
   }

   close(cmp_fd);
   close(urand_fd);

   LOG("test results:\n");
   eprintf(" - successes:\t%d\n - failures:\t%d\n", nsuccess, nfail);

   exit(0);

}

void read_mirror(mirrored_ptr_t *dst, int src_fd) {
   size_t size = dst->size;
   char *dst_ptr = dst->ptr;
   int dst_fd = dst->filedes;
   char buf[size];
   char *buf_it = buf;
   do {
      ssize_t bytes_read;
      if ((bytes_read = read(src_fd, buf_it, (size_t) (size - (buf_it - buf)))) < 0) {
         perror("read");
         exit(2);
      }
      buf_it += bytes_read;
   } while (buf_it != buf + size);
   
   memcpy(dst_ptr, buf, size);
   
   /* save file offset in comparison file */
   dst->offset = lseek(dst_fd, 0, SEEK_END);
   
   /* write data to comparison file */
   buf_it = buf;
   do {
      ssize_t bytes_written;
      if ((bytes_written = write(dst_fd, buf_it, (size_t) (size - (buf_it - buf)))) < 0) {
         perror("write");
         exit(3);
      }
      buf_it += bytes_written;
   } while (buf_it != buf + size);   
}

bool validate(mirrored_ptr_t *mirptr) {
   char *ptr = mirptr->ptr;
   size_t size = mirptr->size;
   int filedes = mirptr->filedes;
   off_t offset = mirptr->offset;
   char buf[size];
   char *buf_it;
   
   /* read in corresponding data from file */
   lseek(filedes, offset, SEEK_SET);

   buf_it = buf;
   do {
      ssize_t bytes_read;
      if ((bytes_read = read(mirptr->filedes, buf_it, (size_t) (size - (buf_it - buf)))) < 0) {
         perror("read");
         exit(2);
      }
      buf_it += bytes_read;
   } while (buf_it != buf + size);
   
   /* compare file buffer vs. malloc'ed heap pointer */
   if (memcmp(ptr, buf, size)) {
      /* memory differs! */
      eprintf("test: validate: heap data at %p corrupted!\n", ptr);
      return false;
   }

   return true;
}

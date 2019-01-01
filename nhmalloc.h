// nhmalloc.h
// contains macros for memory alignment in malloc implementation

#ifndef __MYMALLOC_H
#define __MYMALLOC_H

#include <stddef.h>

#define MALLOC_ALIGN 16
#define MALLOC_LOG_ALIGN 4
#define MALLOC_ALIGN_MASK (~((unsigned) 0) << MALLOC_LOG_ALIGN)
#define MALLOC_ALIGN_SIZE(size) (((size-1) & MALLOC_ALIGN_MASK) \
                                 + MALLOC_ALIGN)
#define MALLOC_ALIGN_PTR(ptr) ((void *) MALLOC_ALIGN_SIZE((uint64_t) ptr))
#define MALLOC_ALIGN_VALIDATE(val) (!((size_t) val & ~MALLOC_ALIGN_MASK))

#endif

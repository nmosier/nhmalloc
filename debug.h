// debug.h
// includes macros for debugging my-malloc

#ifndef __DEBUG_H
#define __DEBUG_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define DEBUG 1

#define COLOR_RED   "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

#define LOG(str) write(STDERR_FILENO, str, strlen(str))
#define eprintf(fmt, ...) { char eprintf_buf[1000]; sprintf(eprintf_buf, fmt, __VA_ARGS__); \
      LOG(eprintf_buf); }


#endif

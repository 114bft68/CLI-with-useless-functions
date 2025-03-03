#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int prompt(char* string, const int length);
int alloc(void** ptr, const size_t size);
int freealloc(void* ptr);
int openf(FILE** ptr, const char* path, const char* mode);
int closef(FILE* ptr);
void readyToExit(void);

#endif
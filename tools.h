#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int prompt(char* string, int number);
int alloc(void** ptr, size_t size);
void freealloc(void** ptr);
int openf(FILE** ptr, char* path, char* mode);
int closef(FILE** ptr);
void readyToExit();

#endif
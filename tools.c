#include "tools.h"

static void** toBeFreed[10];
static FILE** toBeClosed[5];
static int allocCount = 0;
static int openCount = 0;

int prompt(char* string, int length) {
    if (string) {
        if (!fgets(string, length, stdin)) {
            perror("\nUnable to read input");
            return 1;
        } else {
            if (strlen(string) == length - 1 && string[length - 2] != '\n') {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) {}
            }
            string[strcspn(string, "\n")] = '\0';
        }
        return 0;
    }
    return 1;
}

int alloc(void** ptr, size_t size) {
    if (ptr) {
        if (!(*ptr = malloc(size))) {
            return 1;
        }
        toBeFreed[allocCount++] = ptr;
        return 0;
    }
    return 1;
}

void freealloc(void** ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

int openf(FILE** ptr, char* path, char* mode) {
    if (ptr) {
        if (!(*ptr = fopen(path, mode))) {
            perror("\nUnable to open a file");
            return 1;
        }
        toBeClosed[openCount++] = ptr;
        return 0;
    }
    return 1;
}

int closef(FILE** ptr) {
    if (ptr && *ptr) {
        if (fclose(*ptr)) {
            perror("\nUnable to close a file");
            return 1;
        }
        return 0;
    }
    return 1;
}

void readyToExit() {
    for (int i = 0; i < allocCount; i++) {
        freealloc(toBeFreed[i]);
    }
    for (int i = 0; i < openCount; i++) {
        closef(toBeClosed[i]);
    }
}
#include "tools.h"

#define maxAlloc 20 // change this value
#define maxOpenf 20  // change this value

static void* toBeFreed[maxAlloc];
static FILE* toBeClosed[maxOpenf];
static int allocCount = 0;
static int openCount = 0;

int prompt(char* string, const int length) {
    if (string) {
        if (!fgets(string, length, stdin)) {
            printf("\nUnable to read input\n");
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

int alloc(void** ptr, const size_t size) {
    if (allocCount <= maxAlloc - 1) {
        if (ptr) {
            if (!(*ptr = malloc(size))) {
                printf("\nUnable to allocate memory\n");
                return 1;
            }
            toBeFreed[allocCount++] = *ptr;
            return 0;
        }
    } else {
        printf("\nPlease adjust the relevant value (maxAlloc) in \"tools.c\"\n");
    }
    return 1;
}

int freealloc(void* ptr) {
    if (ptr) {
        int index = -1;
        for (int i = 0; i < allocCount; i++) {
            if (toBeFreed[i] == ptr) {
                index = i;
                break;
            }
        }

        free(ptr);
        ptr = NULL;

        if (index == -1) return 1;

        for (int i = index; i < allocCount - 1; i++) {
            toBeFreed[i] = toBeFreed[i + 1];
            toBeFreed[i + 1] = NULL;
        }

        if (allocCount > 0) --allocCount;

        return 0;
    }
    return 1;
}

int openf(FILE** ptr, const char* path, const char* mode) {
    if (openCount <= maxOpenf - 1) {
        if (ptr) {
            if (!(*ptr = fopen(path, mode))) {
                perror("\nUnable to open a file");
                return 1;
            }
            toBeClosed[openCount++] = *ptr;
            return 0;
        }
    } else {
        printf("\nPlease adjust the relevant value (maxOpenf) in \"tools.c\"\n");
    }
    return 1;
}

int closef(FILE* ptr) {
    if (ptr) {
        int index = -1;
        for (int i = 0; i < openCount; i++) {
            if (toBeClosed[i] == ptr) {
                index = i;
                break;
            }
        }

        if (fclose(ptr)) {
            printf("\nUnable to close a file\n");
            return 1;
        }
        ptr = NULL;

        if (index == -1) return 1;

        for (int i = index; i < openCount - 1; i++) {
            toBeClosed[i] = toBeClosed[i + 1];
            toBeClosed[i + 1] = NULL;
        }

        if (openCount > 0) --openCount;

        return 0;
    }
    return 1;
}

void readyToExit(void) {
    while (allocCount > 0) {
        freealloc(toBeFreed[--allocCount]);
    }
    while (openCount > 0) {
        closef(toBeClosed[--openCount]);
    }
}
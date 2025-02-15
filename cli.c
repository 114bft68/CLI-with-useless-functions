#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "tools.h"

#define CMD_LENGTH  17 // max length for each command + \n\0
#define PATH_LENGTH 102 // max length for each file path + \n\0
#define exitm(one) if (one) { readyToExit(); return 1; } // exit a non-void function with this macro

const char* COMMANDS[] = {
    "help",
    "prefix",
    "swapfiles",
    "mergefiles",
    "deletefile",
    "clear",
    "exit"
};

#define COMMANDS_COUNT (sizeof(COMMANDS) / sizeof(COMMANDS[0]))

char ActualCommands[COMMANDS_COUNT][CMD_LENGTH];

int commandIndex(char* cmd);
void sendHelp();
int changePrefix(int n, char* str);
int filesTreatments(int mode); // swap (0) || merge (1)
int deletefile();

struct files {
    char path[PATH_LENGTH];
    FILE* open;
    long charsCount;
    char* content;
    char mode[3]; // (wb || rb || ab) + \0
};

int end = 0;
int main() {
    changePrefix(1, "--");

    char command[CMD_LENGTH];
    printf("========= FAKE CMD =========\n");
    sendHelp();

    while (!end) {
        printf("\n> ");
        exitm(prompt(command, CMD_LENGTH));

        switch (commandIndex(command)) {
            case 0:
                sendHelp();
                break;
            case 1:
                changePrefix(0, "--");
                break;
            case 2:
                printf("\n** Only swap files with the same extension **\n");
                filesTreatments(0);
                break;
            case 3:
                printf("\n** Merging non-text files may not work **\n");
                filesTreatments(1);
                break;
            case 4:
                deletefile();
                break;
            case 5:
                #if defined(_WIN32)
                    system("cls");
                #else
                    system("clear");
                #endif
                printf("========= FAKE CMD =========\n");        
                break;
            case 6:
                end = 1;
                break;
            default:
                printf("\nInvalid command! Use %s\n", ActualCommands[0]);
                break;
        }
    }

    readyToExit();
    return 0;
}

int commandIndex(char* cmd) {
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        if (!strcmp(cmd, ActualCommands[i])) {
            return i;
        }
    }
    return -1;
}

void sendHelp() {
    printf("\nAvailable commands:\n");
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        printf("%s\n", ActualCommands[i]);
    }
}

int changePrefix(int n, char* str) {
    char* prefix;
    exitm(alloc((void**) &prefix, 4)); // 2 characters \n\0
    
    if (n) {
        strcpy(prefix, str);
    } else {
        printf("\nPrefix (default: --):\n>> ");
        exitm(prompt(prefix, 4)); // get input
    }

    for (int i = 0; i < COMMANDS_COUNT; i++) {
        sprintf(ActualCommands[i], "%s%s", prefix, COMMANDS[i]);
    }

    if (!n) printf("\nSuccessfully changed the prefix\n");
    freealloc((void**) &prefix);
}

int filesTreatments(int mode) {
    struct files file[3]; // file0, file1, file2 (may not be used)
    int availableFiles = 2;

    printf("\nPlease input the first file path:\n>> ");
    exitm(prompt(file[0].path, PATH_LENGTH));
    printf("\nPlease input the second file path:\n>> ");
    exitm(prompt(file[1].path, PATH_LENGTH));

    strcpy(file[0].mode, "rb"); // assuming the function is "--swapfiles"
    strcpy(file[1].mode, "rb"); // assuming the function is "--swapfiles"
    
    if (mode) {
        char charNum[3]; // 1 digit number + \n\0
        int num = 0;
        
        while (num < 1 || num > 3) {
            printf("\n--- Options ---\n[1] Append File2 to File1\n[2] Append File1 to File2\n[3] Append File1 and File2 to File3\n\n>>> ");
            exitm(prompt(charNum, 3));
            num = atoi(charNum);
        }

        if (num == 3) {
            printf("\nPlease input the third file path:\n>> ");
            exitm(prompt(file[2].path, PATH_LENGTH));
            availableFiles = 3;
        }

        // num3 -> file2, num2 -> file1, num1 -> file0
        for (int i = 0; i < 3; i++) {
            if (num == i + 1) {
                strcpy(file[i].mode, "ab");
            }
        }
    }

    for (int i = 0; i < availableFiles; i++) {
        exitm(openf(&file[i].open, file[i].path, file[i].mode));
        if (!strcmp(file[i].mode, "rb")) {
            
            errno = 0;
            if ((fseek(file[i].open, 0, SEEK_END) && errno) ||
               ((file[i].charsCount = ftell(file[i].open)) == -1L && errno) ||
                (fseek(file[i].open, 0, SEEK_SET) && errno)) {

                perror("\nAn error has occurred");
                exitm(1);
            }

            exitm(alloc((void**) &file[i].content, (size_t) file[i].charsCount));

            if (fread(file[i].content, sizeof(char), (size_t) file[i].charsCount, file[i].open) != (size_t) file[i].charsCount) {
                perror("\nUnable to read file completely (fread)");
                exitm(1);
            }
        }
    }

    if (!mode) {
        exitm(closef(&file[0].open) || closef(&file[1].open) ||
        openf(&file[0].open, file[0].path, "wb") || openf(&file[1].open, file[1].path, "wb"));

        // !mode => either 0 or 1
        if (fwrite(file[0].content, sizeof(char), (size_t) file[0].charsCount, file[1].open) != (size_t) file[0].charsCount ||
            fwrite(file[1].content, sizeof(char), (size_t) file[1].charsCount, file[0].open) != (size_t) file[1].charsCount)
        {
            perror("\nUnable to write to file(s)");
            exitm(1);
        }
    } else {

        int target, n = 0;
        for (int i = 0; i < availableFiles; i++) {
            if (!strcmp(file[i].mode, "ab")) {
                target = i;
                break;
            }
        }

        for (int n = 0; n < availableFiles; n++) {
            if (n == target) {
                continue;
            }
            
            if (fwrite(file[n].content, sizeof(char), (size_t) file[n].charsCount, file[target].open) != (size_t) file[n].charsCount) {
                printf("\nUnable to write to File%d: %s\n", target, strerror(errno));
                exitm(1);
            }
        }
    }

    printf("\nSuccess!\n");
    readyToExit(); // the function frees memory and closes files for me
    return 0;
}

int deletefile() {
    struct files FileToDelete[2];
    strcpy(FileToDelete[0].mode, "rb");
    strcpy(FileToDelete[1].mode, "wb");

    printf("\nPlease input the path of the file to be deleted:\n>> ");
    exitm(prompt(FileToDelete[0].path, PATH_LENGTH));

    exitm(openf(&FileToDelete[0].open, FileToDelete[0].path, FileToDelete[0].mode));

    errno = 0;
    if ((fseek(FileToDelete[0].open, 0, SEEK_END) && errno) ||
        ((FileToDelete[0].charsCount = ftell(FileToDelete[0].open)) && errno)) {
        // SEEK_SET is not needed as the file will be deleted soon
        perror("\nAn error has occured");
        exitm(1);
    }

    exitm(closef(&FileToDelete[0].open));

    int error;
    
    if (error = remove(FileToDelete[0].path)) exitm(printf("\nFailed to delete the file: %s\n", strerror(error)) || 1);
    // if there's an error => printf, then exitm(1)

    exitm(openf(&FileToDelete[1].open, FileToDelete[0].path, FileToDelete[1].mode));

    srand(time(NULL));

    for (long i = 0; i < FileToDelete[0].charsCount; i++) {
        if (fputc('a' + rand() % 26, FileToDelete[1].open) == EOF) { // random character from a - z (a + (0 to 25))
            perror("\nAn error has occurred");
            exitm(1);
        }
    }

    exitm(closef(&FileToDelete[1].open));

    if (error = remove(FileToDelete[0].path)) exitm(printf("\nFailed to delete the file: %s\n", strerror(error)) || 1);

    printf("\nSuccess!\n");
    readyToExit();
    return 0;
}
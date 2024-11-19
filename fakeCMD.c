#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"

#define CMD_LENGTH  17 // max length for each command + \n\0
#define PATH_LENGTH 102 // max length for each file path + \n\0
#define exitm(one) if (one) { readyToExit(); return 1; } // exit a non-void function with this macro

const char* COMMANDS[] = {
    "--help",
    "--swapfiles",
    "--mergefiles",
    "--tictactoe",
    "--clear",
    "--exit"
};

const int COMMANDS_COUNT = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

int commandIndex(char* cmd);
void sendHelp();
int filesTreatments(int mode); // merge (0) || swap (1)
int ticTacToe();

struct files {
    char path[PATH_LENGTH];
    FILE* open;
    long charsCount;
    char* content;
    char mode[3]; // (wb || rb || ab) + \0
};

int end = 0;
int main() {
    char command[CMD_LENGTH];
    printf("========= FAKE CMD =========\n");
    sendHelp();

    while (!end) {
        printf("\n> ");
        prompt(command, CMD_LENGTH);

        switch (commandIndex(command)) {
            case 0:
                sendHelp();
                break;
            case 1:
                filesTreatments(0);
                break;
            case 2:
                filesTreatments(1);
                break;
            case 3:

                break;
            case 4:
                #if defined(_WIN32)
                    system("cls");
                #else
                    system("clear");
                #endif
                printf("========= FAKE CMD =========\n");        
                break;
            case 5:
                end = 1;
                break;
            default:
                printf("\nInvalid command! Use --help\n");
                break;
        }
    }

    return 0;
}

int commandIndex(char* cmd) {
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        if (!strcmp(cmd, COMMANDS[i])) {
            return i;
        }
    }
    return -1;
}

void sendHelp() {
    printf("\nAvailable commands:\n");
    for (int i = 0; i < COMMANDS_COUNT; i++) {
        printf("%s\n", COMMANDS[i]);
    }
}

int filesTreatments(int mode) {
    struct files file[3]; // file0, file1, file2 (may not be used)
    int availableFiles = 2;

    printf("\nPlease input the first file path:\n>> ");
    prompt(file[0].path, PATH_LENGTH);
    printf("\nPlease input the second file path:\n>> ");
    prompt(file[1].path, PATH_LENGTH);

    strcpy(file[0].mode, "rb"); // assuming the function is "--swapfiles"
    strcpy(file[1].mode, "rb"); // assuming the function is "--swapfiles"
    
    if (mode) {
        char charNum[3]; // 1 digit number + \n\0
        int num = 0;
        
        while (num < 1 || num > 3) {
            printf("\n--- Options ---\n[1] Append File2 to File1\n[2] Append File1 to File2\n[3] Append File1 and File2 to File3\n\n>>> ");
            prompt(charNum, 3);
            num = atoi(charNum);
        }

        if (num == 3) {
            printf("\nPlease input the third file path:\n>> ");
            prompt(file[2].path, PATH_LENGTH);
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

            if (fseek(file[i].open, 0, SEEK_END) ||
                (file[i].charsCount = ftell(file[i].open)) == -1L ||
                fseek(file[i].open, 0, SEEK_SET))
            {
                printf("\nAn error has occurred %s: %s\n", file[i].charsCount == -1L ? "(ftell)" : "(fseek)", strerror(errno));
                exitm(1);
            }

            file[i].content = malloc(sizeof(char) * file[i].charsCount);
            if (fread(file[i].content, sizeof(char), (size_t) file[i].charsCount, file[i].open) != (size_t) file[i].charsCount) {
                perror("\nUnable to read file completely (fread)");
                exitm(1);
            }
        }
    }

    if (!mode) {
        for (int i = 0; i < 2; i++) {
            if (!freopen(file[i].path, "wb", file[i].open)) {
                perror("\nUnable to reopen file (freopen)");
                exitm(1);
            }
        }
        // !mode => either 0 or 1
        for (int i = 0; i < 2; i++) {
            if (fwrite(file[i].content, sizeof(char), (size_t) file[i].charsCount, file[i ^ 1].open) != (size_t) file[i].charsCount) {
                printf("\nUnable to write to File%d: %s\n", i + 1, strerror(errno));
                exitm(1);
            }
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
    readyToExit(); // the function frees memory for me
    return 0;
}
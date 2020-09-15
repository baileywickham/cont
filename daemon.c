#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contd.h"

#define MAXLINE 256

int parseline(char*);
int splitonspaces(char*, char*[10]);

int main(int argc, char** argv)
{
    char line[MAXLINE];

    printf(":|: ");
    fflush(stdout);
    while (fgets(line, MAXLINE, stdin)) {
        if (parseline(line) < 0) {
            exit(EXIT_FAILURE);
        }
        printf(":|:");
        fflush(stdout);
    }
}

int parseline(char* line) {
    char* parsed[10]; // 10 paramaters of 36 char len"
    int len=0;
    for (int i=0; i < 10; i++) {
        parsed[i] = malloc(36);
    }

    // lmao yikes
    len = splitonspaces(line, parsed);
    if (len == 0) {
        return 0;
    }

    if (!strcmp(parsed[0], "create")) {
        if (len != 2) {
            printf("create <name>\n");
            return 0;
        }
        createcont(parsed[1]);
        return 0;

    } else if (!strcmp(parsed[0], "run")) {
    }

    return -1;
}

int splitonspaces(char* line, char* parsed[10]) {
    char* r = line;
    char* token;
    int i = 0;
    while ((token=strtok_r(r, " \n", &r))) {
        strcpy(parsed[i++], token);
    }
    return i;
}

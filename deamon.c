#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 256

int parseline(char*);

int main(int argc, char** argv)
{
    char line[MAXLINE];

    printf(":|: ");
    fflush(stdout);
    while (fgets(line, MAXLINE, stdin)) {
        if (parseline(line) < 0) {
            exit(EXIT_FAILURE);
        }

    }
}

int parseline(char* line)
{
    char* r;
    char* token;

    if (line[0] == '\n') {
        return 0;
    }

    token = strtok_r(line, " \n", &r);
    if (!strcmp(token, "create")) {
        printf("%s", token);
    } else if (!strcmp(token, "run")) {
        printf("%s", token);
    }

    return -1;
}

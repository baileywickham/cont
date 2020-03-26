#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>

#include "contd.h"

#define NEWROOT "newroot"
#define OLDROOT "oldroot"

void usage();

const char* MOUNT_DIR = "./newroot";

int main(int argc, char** argv)
{
    int opt;

    if (argc < 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "s")) != -1) {
        switch (opt) {
        case 's':
            startcont();
            break;
        default:
            usage();
        }
    }
}

void usage()
{
    printf("cont -s <program> <args...>\n");
}

void execcmd(char** argv)
{
}

void entercont()
{
    // if (setns(int __fd, int __nstype) < 0) {
    //     perror("entering cont failed");
    // }
}

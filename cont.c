#define _GNU_SOURCE
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mount.h>
#include <sys/syscall.h>

void usage();
void unsharecont();

const char* MOUNT_DIR = "./newroot";

int main(int argc, char** argv)
{
    pid_t pid;
    int status;

    if (argc == 1) {
        usage();
        exit(1);
    }

    unsharecont(); // isolate container
    pid = fork();
    if (pid == -1) {
        printf("can't fork, error occured\n");
        exit(1);
    } else if (pid == 0) {
        char** argv_list = &argv[1];
        if (execvp(argv[1], argv_list) < 0) {
            // catch exec error
            perror("");
            exit(1);
        }
        exit(0);
    } else {
        if (waitpid(pid, &status, 0) > 0) {
            if (!(WIFEXITED(status) && !WEXITSTATUS(status))) {
                printf("failed\n");
                exit(1);
            }
        }
        exit(0);
    }
    return 0;
}

void usage()
{
    printf("cont <program> <args...>\n");
}


void unsharecont() {
    const int UNSHARE_FLAGS = CLONE_NEWNET |CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWPID;
    const char* newname = "cont"; // TODO change to proc name with hash
    if (unshare(UNSHARE_FLAGS) < 0) {
        perror("failed to change namespaces\n");
        exit(1);
    }
    if (sethostname(newname, strlen(newname)) < 0) {
        perror("failed to rename cont\n");
        exit(1);
    }
    if (chroot(MOUNT_DIR) < 0 || chdir(MOUNT_DIR) < 0) {
        perror("chroot failed");
    }
}


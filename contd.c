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
#define MOUNT_DIR "./workdir"

void startcont()
{
    char* args[1];

    pid_t pid = getpid();
    FILE* f = fopen("/run/contd.pid", "w");
    if (f == NULL) {
        perror("failed to write contd.pid");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%d\n", pid);
    fclose(f);

    createdaemon();
    unsharecont();

    args[0] = NULL;
    // exec init process
    // TODO change to command
    execv("/sbin/init", args);
}

void unsharecont()
{
    const int UNSHARE_FLAGS = CLONE_NEWNET | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWPID;
    const char* newname = "cont"; // TODO change to proc name with hash
    if (unshare(UNSHARE_FLAGS) < 0) {
        perror("failed to change namespaces\n");
        exit(EXIT_FAILURE);
    }
    if (sethostname(newname, strlen(newname)) < 0) {
        perror("failed to rename cont\n");
        exit(EXIT_FAILURE);
    }
    if (mount("overlay", "./workdir", "overlayfs", MS_MGC_VAL, "lowerdir=./newroot,upperdir=./oldroot") < 0) {
        perror("mount overlay failed");
    }
    if (chroot(MOUNT_DIR) < 0 || chdir(MOUNT_DIR) < 0) {
        perror("chroot failed");
        exit(EXIT_FAILURE);
    }
    // TODO Figure out how this works
    // if (mount("", "/", "none", MS_REC | MS_PRIVATE, NULL) < 0) {
    //     perror("mount failed");
    //     exit(EXIT_FAILURE);
    // }
}

void createdaemon()
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("failed to fork daemon");
    }
    if (pid > 0) {
        exit(0); // kill parent
    }
    if (setsid() < 0) {
        perror("setting session id failed");
        exit(EXIT_FAILURE);
    }
    // ignore signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0) {
        perror("failed to fork child daemon");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(0); //kill parent
    }
}

void mountproc()
{
    if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
        perror("failed to mount proc");
        exit(EXIT_FAILURE);
    }
}

void pvtroot()
{
    // TODO decide whether to use pivot root or switchroot
}

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
#include <syslog.h>

#include "contd.h"


#define NEWROOT "newroot"
#define OLDROOT "oldroot"
#define MOUNT_DIR "./newroot"


void startcont() {
    createdaemon();
    unsharecont();
}


void unsharecont() {
    const int UNSHARE_FLAGS = CLONE_NEWNET |CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWPID;
    const char* newname = "cont"; // TODO change to proc name with hash
    if (unshare(UNSHARE_FLAGS) < 0) {
        perror("failed to change namespaces\n");
        exit(EXIT_FAILURE);
    }
    if (sethostname(newname, strlen(newname)) < 0) {
        perror("failed to rename cont\n");
        exit(EXIT_FAILURE);
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


void createdaemon() {
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


void mountproc() {
    if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
        perror("failed to mount proc");
        exit(EXIT_FAILURE);
    }
}

void pvtroot() {
    // TODO decide whether to use pivot root or switchroot
}


void entercont() {
   // if (setns(int __fd, int __nstype) < 0) {
   //     perror("entering cont failed");
   // }
}


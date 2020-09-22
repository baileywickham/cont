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
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "contd.h"

#define NEWROOT "newroot"
#define OLDROOT "oldroot"
#define WORKDIR "./workdir"

void runcont(char*, int, char**);

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("contd <cmd> ...\n");
        exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[1], "create")) {
        if (argc != 3) {
            printf("contd create <cont>\n");
            return 0;
        }
        createcont(argv[2]);
        return 0;

    } else if (!strcmp(argv[1], "run")) {
        if (argc != 4) {
            // [0 contd] [1 run] [2 contname] [3... cmd]
            printf("contd run <cont> <cmd>\n");
            return 0;
        }
        runcont(argv[2], argc-1, &argv[3]);
    }

}

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

void createcont(char* contname) {
    char d[25], u[25], w[25],o[25];
    sprintf(d, "./containers/%s", contname);
    sprintf(u, "./containers/%s/upper", contname);
    sprintf(w, "./containers/%s/work", contname);
    sprintf(o, "./containers/%s/overlay", contname);
    if (mkdir(d, 0755) < 0 && errno == EEXIST) {
        printf("container %s already exists\n", contname);
    }
    if (mkdir(u, 0755) < 0 || mkdir(w, 0755) < 0 || mkdir(o, 0755) < 0){
        perror("failed to create directory");
        exit(EXIT_FAILURE);
    }
    printf("Container created at %s\n", d);

}

void runcont(char* contname, int argc, char** argv) {
    unsharecont(contname);
    // _argv is by default null terminated
    // exec v: vector, p: check path
    execvp(argv[0], argv);
}

void unsharecont(char* contname) {
    const int UNSHARE_FLAGS = CLONE_NEWNET | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWPID;
    char mountdir[100];
    char mountparam[100];

    if (unshare(UNSHARE_FLAGS) < 0) {
        perror("failed to change namespaces\n");
        exit(EXIT_FAILURE);
    }
    if (sethostname(contname, strlen(contname)) < 0) {
        perror("failed to rename cont\n");
        exit(EXIT_FAILURE);
    }

    sprintf(mountparam, "lowerdir=alpine,upperdir=/home/y/workspace/cont/containers/%s/upper,workdir=containers/%s/work", contname,contname);
    sprintf(mountdir, "containers/%s/overlay",contname);

    if (mount("overlay", mountdir, "overlay", MS_MGC_VAL, mountparam) < 0) {
        // This will fail on kerel 5.4
        perror("mount overlay failed");
        exit(EXIT_FAILURE);
    }

    if (chroot(mountdir) < 0 || chdir(mountdir) < 0) {
        perror("chroot failed");
        exit(EXIT_FAILURE);
    }
    // TODO Figure out how this work
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

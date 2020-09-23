#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <syslog.h>
#include <unistd.h>

#include "contd.h"

#define MAXCONTNAMELEN 36


int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("contd <cmd> ...\n");
        exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[1], "create")) {
        if (argc != 3) {
            printf("contd create <cont>\n");
            return 0;
        }
        if (strlen(argv[2]) >= MAXCONTNAMELEN) {
            printf("Container name exceeds max len: %d", MAXNAMLEN);
            exit(EXIT_FAILURE);
        }
        createcont(argv[2]);
        return 0;

    } else if (!strcmp(argv[1], "run")) {
        if (argc != 4) {
            // [0 contd] [1 run] [2 contname] [3... cmd]
            printf("contd run <cont> <cmd>\n");
            return 0;
        }
        runcont(argv[2], argc - 1, &argv[3]);
    }
}

int direxists(char* d) {
    DIR* dir = opendir(d);
    if (dir) {
        closedir(dir);
        return 1;
    } else {
        return 0;
    }
}

void createcgroup() {
    // Called once on first run of this program.
    // Creates the cgroup for all child proccesses.
    char* d = "/sys/fs/cgroup/memory/cont";
    if (mkdir(d, 0755) < 0)
        perror("Failed to create cgroup");
    FILE* f = fopen("/sys/fs/cgroup/memory/cont/memory.limit_in_bytes", "w");
    // about a gig, I think. Either that or 200MB
    fprintf(f, "200000000\n");
    fclose(f);
}

int cgroupexists() {
    return direxists("/sys/fs/cgroup/memory/cont");
}


void entercgroup(char* contname) {
    pid_t pid = getpid();
    FILE* f = fopen("/run/contd.pid", "w");
    if (f == NULL) {
        perror("failed to write contd.pid");
        exit(EXIT_FAILURE);
    } // Keeping this here, this writes the pid of the proc to run
    fprintf(f, "%d\n", pid);
    fclose(f);
}

void cleancgroup(pid_t pid) {
    char buff[256];
}
void addproctocgroup(pid_t pid) {
    FILE* f = fopen("/sys/fs/cgroup/memory/cont/memory.procs", "a");
    if (f == NULL) {
        perror("failed to write memory.procs");
        exit(EXIT_FAILURE);
    }
    fprintf(f, "%d\n", pid);
    fclose(f);
}


int contexists(char* contname)
{
    char d[MAXCONTNAMELEN];
    sprintf(d, "containers/%s", contname);
    return direxists(d);
}

void createcont(char* contname) {
    if (!cgroupexists())
        createcgroup();

    char d[MAXCONTNAMELEN], u[MAXCONTNAMELEN], w[MAXCONTNAMELEN], o[MAXCONTNAMELEN];
    sprintf(d, "./containers/%s", contname);
    sprintf(u, "./containers/%s/upper", contname);
    sprintf(w, "./containers/%s/work", contname);
    sprintf(o, "./containers/%s/overlay", contname);
    if (contexists(contname)) {
        printf("container %s already exists\n", contname);
        exit(EXIT_FAILURE);
    }
    if (mkdir(d, 0755) < 0 || mkdir(u, 0755) < 0 || mkdir(w, 0755) < 0 || mkdir(o, 0755) < 0) {
        perror("failed to create directory");
        exit(EXIT_FAILURE);
    }
    printf("Container created at %s\n", d);
}


void runcont(char* contname, int argc, char** argv)
{
    if (!contexists(contname)) {
        printf("container %s doesn't exist\n", contname);
        exit(EXIT_FAILURE);
    }

    unsharecont(contname);
    mountproc();
    // _argv is by default null terminated
    // exec v: vector, p: check path
    pid_t pid = fork();
    if (pid < 0) {
        perror("failed to fork daemon");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // Parent
        addproctocgroup(pid);
        // Need to wait here
        cleancgroup(pid);
    } else  {
        // Child
        execvp(argv[0], argv);
    }
    // Not sure if we need to start the init proc
    // execv("/sbin/init", args);
}

void unsharecont(char* contname)
{
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

    sprintf(mountparam, "lowerdir=alpine,upperdir=containers/%s/upper,workdir=containers/%s/work", contname, contname);
    sprintf(mountdir, "containers/%s/overlay", contname);

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

void mountproc() {
    if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
        perror("failed to mount proc");
        exit(EXIT_FAILURE);
    }
}

void pvtroot()
{
    // TODO decide whether to use pivot root or switchroot
}

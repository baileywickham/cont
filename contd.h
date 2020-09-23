#include <sys/types.h>
void unsharecont();
void mountproc();
void createdaemon();
void startcont();
void createcont(char*);
int cgroupexists();
int direxists(char*);
int contexists(char*);
void createcgroup();
void runcont(char*, int, char**);
void cleancgroup(pid_t);

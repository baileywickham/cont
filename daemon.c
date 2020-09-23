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

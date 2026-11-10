// Expose POSIX/BSD APIs (setsid, sigaction, ...) under -std=c99.
#define _DEFAULT_SOURCE

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

static void exec_program(void) {
    while (running) {
        syslog(LOG_INFO, "Hello %s", "World");
        sleep(1);
    }
}

static void fork_and_exit_parent(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // parent leaves
    }
    // child has pid == 0
}

static void daemonize(void) {
    // Flush pending output so it isn't duplicated into the child.
    fflush(NULL);

    // 1st fork: return control to the shell; child is not a process group leader.
    fork_and_exit_parent();

    // New session: detach from the controlling terminal.
    if (setsid() < 0) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    // Ignore SIGHUP sent when the session leader exits.
    signal(SIGHUP, SIG_IGN);

    // 2nd fork: no longer session leader, so it can never reacquire a terminal.
    fork_and_exit_parent();

    // Reset file mode mask and move off any mounted filesystem.
    umask(0);
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    // Close every inherited file descriptor.
    for (long fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
        close((int)fd);
    }

    // Reopen stdin/stdout/stderr on /dev/null (fds 0, 1, 2).
    int fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO) {
        exit(EXIT_FAILURE);
    }
    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0) {
        exit(EXIT_FAILURE);
    }
}

/**
 * log into journalctl
 * journalctl -t hello -b
 */
int main(void) {
    daemonize();

    openlog("hello", LOG_PID, LOG_DAEMON);

    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    syslog(LOG_NOTICE, "daemon started");
    exec_program();
    syslog(LOG_NOTICE, "daemon stopping");

    closelog();
    return EXIT_SUCCESS;
}

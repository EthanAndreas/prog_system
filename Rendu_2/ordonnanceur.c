#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHK(op)            \
    do {                   \
        if ((op) == -1)    \
            raler(1, #op); \
    } while (0)

noreturn void raler(int syserr, const char *msg, ...) {
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    if (syserr == 1)
        perror("");

    exit(EXIT_FAILURE);
}

#define END_PROC -3

volatile sig_atomic_t begin_process = 0, end_process = 0,
                      end_alarm = 0, term_process = 0;

void handle_sig(int sig) {

    switch (sig) {

    case SIGUSR1:
        begin_process = 1;
        break;

    case SIGUSR2:
        end_process = 1;
        break;

    case SIGALRM:
        end_alarm = 1;
        break;

    case SIGCHLD:
        term_process = 1;
        break;
    }
}

void control_process(int nb_proc, int nb_qtm, pid_t pid) {

    struct sigaction s;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    memset(&s, 0, sizeof(sigaction));
    s.sa_handler = handle_sig;
    s.sa_flags = 0;
    sigemptyset(&s.sa_mask);

    while (nb_qtm > 0) {

        while (begin_process == 1) {

            sigsuspend(&mask);
            if (errno != EINTR)
                raler(1, "sigsuspend");
        }

        begin_process = 0;

        fprintf(stdout, "SURP - process %i\n", nb_proc);
        fflush(stdout);

        while (end_process == 0) {

            // mask
            CHK(sigprocmask(SIG_UNBLOCK, &mask, NULL));

            sleep(1);

            // demask
            CHK(sigprocmask(SIG_BLOCK, &mask, NULL));
        }

        end_process = 0;

        // launch of following process
        // after the end of the process
        CHK(kill(pid, SIGUSR1));

        fprintf(stdout, "EVIP - process %i\n", nb_proc);
        fflush(stdout);

        nb_qtm--;
    }
}

int main(int argc, char **argv) {

    // check command line
    if (argc <= 3)
        raler(1, "Nombre d'argument incorrect");

    for (int i = 1; i < argc; i++) {

        if (atoi(argv[i]) < 1)
            raler(1, "Quantum's time > 1");
    }

    struct sigaction s;
    sigset_t mask, old;

    CHK(sigemptyset(&mask));
    memset(&s, 0, sizeof(sigaction));
    s.sa_handler = handle_sig;
    s.sa_flags = 0;
    CHK(sigemptyset(&s.sa_mask));

    CHK(sigaction(SIGUSR1, &s, NULL));
    CHK(sigaction(SIGUSR2, &s, NULL));
    CHK(sigaction(SIGALRM, &s, NULL));
    CHK(sigaction(SIGCHLD, &s, NULL));

    pid_t pid;
    int status;

    int num_proc = 0;

    pid_t *tab_pid = malloc((argc - 2) * sizeof(pid_t));

    // creation of process with the signal
    for (int i = 0; i < argc - 2; i++) {

        switch ((pid = fork())) {

        case -1:
            raler(1, "fork");

        case 0:
            control_process(i, atoi(argv[i]), pid);
            num_proc++;
            exit(0);

        default:
            tab_pid[i] = pid;

            if (wait(&status) == -1)
                raler(1, "wait");

            if (WIFEXITED(status))
                fprintf(stderr, "exit(%d)\n", WEXITSTATUS(status));
        }
    }

    // scheduling of the process
    int nb_proc_treat = 0;
    int nb_proc = 0;

    // launch of the first process
    CHK(kill(tab_pid[nb_proc], SIGUSR1));

    while (nb_proc_treat < argc - 2) {

        if (tab_pid[nb_proc] != END_PROC) {

            // mask
            CHK(sigprocmask(SIG_BLOCK, &mask, &old));

            // wait
            if (begin_process == 0 && end_process == 0 &&
                end_alarm == 0 && term_process == 0) {

                sigsuspend(&mask);
                if (errno != EINTR)
                    raler(1, "sigsuspend");
            }

            // demask
            CHK(sigprocmask(SIG_SETMASK, &old, NULL));

            // signal management

            if (begin_process == 1) {

                nb_proc++;
                nb_proc = nb_proc % (argc - 2);
                CHK(kill(tab_pid[nb_proc], SIGUSR1));

                alarm(atoi(argv[1]));
                CHK(kill(tab_pid[nb_proc], SIGALRM));
            }

            if (end_alarm == 1) {

                CHK(kill(tab_pid[nb_proc], SIGUSR2));
                end_alarm = 0;
            }
        }

        // end of a son process
        if (term_process == 1) {

            nb_proc_treat++;
            tab_pid[nb_proc] = END_PROC;

            fprintf(stdout, "TERM - process %i\n", nb_proc);
            fflush(stdout);

            term_process = 0;
        }
    }

    free(tab_pid);

    return 0;
}
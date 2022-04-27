#include "../include/TP9.h"

// Exercice 9.1

struct counter {
    volatile sig_atomic_t ctrl_c;
} counter = {0};

void handler_c(int sig) {

    (void)sig;

    fprintf(stdout, "\nSIGINT detected\n");
    counter.ctrl_c++;

    if (counter.ctrl_c >= 5)
        exit(0);
}

// main
//  int main(void) {

//     if (signal(SIGINT, handler_c) == SIG_ERR)
//         raler(1, "signal");

//     while (1)
//       pause(); //attente passive : vérifie condition puis bloque

//     return 0;
// }

// Exercice 9.3

void handler_unknown(int sig) { (void)sig; }

// int main(void) {

//     for (int i = 1; i <= 16; i++) {

//         if (i != SIGKILL && i != SIGSTOP)
//             exit(0);

//         if (signal(i, handler_unknown) == SIG_ERR)
//             raler(1, "signal");

//         psignal(i, "");
//     }
// }

// Exercice 9.5

volatile sig_atomic_t arret = 0;

void traite(int sig) {

    (void)sig;
    fprintf(stdout, "+1 seconde\n");
}

void stop(int sig) { (void)sig; }

/*
int main(void) {

    switch (fork()) {

    case -1:
        raler(1, "fork");

    case 0:
        unsigned int alarme = alarm(1);

        if (signal(SIGALRM, traite) == SIG_ERR)
            raler(1, "signal");

        if (arret == 1) {

            fprintf(stdout, "Fin du programme\n");
        }

    default:
        unsigned int alarme2 = alarm(10);

        if (signal(SIGUSR1, stop) == SIG_ERR)
            raler(1, "signal");
    }
}
*/

// Exercice 9.7

struct s2 {
    uint32_t count;
    uint32_t count_big;
} s2 = {0, 0};

volatile sig_atomic_t to_write = 0;
volatile sig_atomic_t end = 1;

void add_c(int sig) {

    (void)sig;
    to_write = 1;
}

void end_term(int sig) {

    (void)sig;
    end = 0;
}

void count_inf(void) {

    int fd;
    CHK((fd = open("toto", O_WRONLY | O_CREAT | O_APPEND, 0666)));

    // sigset_t mask, old;

    struct sigaction s;
    struct sigaction s1;

    const time_t timep;

    while (end == 1) {

        // signal mask (useless here)
        // sigemptyset(&mask);
        // sigaddset(&mask,SIGINT);
        // sigaddset(&mask,SIGTERM);

        // if (sigprocmask(SIG_BLOCK, &mask, &old) == -1)
        //     raler(1,"sigprocmask");

        // critic section
        if (s2.count == UINT32_MAX) {

            s2.count = 0;
            s2.count_big++;
        } else {

            s2.count++;
        }

        if (ctime(&timep) == NULL)
            raler(1, "ctime");

        s.sa_handler = add_c;
        s.sa_flags = 0;
        sigemptyset(&s.sa_mask);
        sigaddset(&s.sa_mask, SIGINT);

        sigaction(SIGINT, &s, NULL);

        if (to_write == 1) {

            CHK((write(fd, &timep, sizeof(time_t))));
            CHK((write(fd, "\n", 1)));
            CHK(write(fd, &s2.count, sizeof(uint32_t)));
        }

        to_write = 0;

        s1.sa_handler = end_term;
        s1.sa_flags = 0;
        sigemptyset(&s1.sa_mask);
        sigaddset(&s1.sa_mask, SIGTERM);

        sigaction(SIGTERM, &s1, NULL);

        // end signal mask
        // if (sigprocmask(SIG_SETMASK, &old, NULL) == -1)
        //     raler(1,"sigprocmask");
    }

    CHK(close(fd));
}

// Exercice 9.10
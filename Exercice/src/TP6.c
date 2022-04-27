#include "../include/TP6.h"

// Exercice 6.1

int processus(void) {

    int raison;
    pid_t pid_fils;
    pid_t pid_pere;

    pid_fils = fork();

    switch (pid_fils) {

    case -1:
        raler(1, "fork");

    case 0:
        pid_pere = getppid();
        pid_fils = getpid();
        printf("pid pere : %ju\n"
               "pid fils : %ju\n",
               (intmax_t)pid_pere, (intmax_t)pid_fils);
        return (pid_fils % 10);

    default:
        CHK(wait(&raison));
        printf("pid fils du pere : %jd\n", (intmax_t)pid_fils);

        if (WIFEXITED(raison))
            printf("exit : %jd\n", (intmax_t)(WEXITSTATUS(raison)));

        return (pid_fils % 10);
    }
}

// Exercice 6.2

int n_processus(int n) {

    pid_t pid;
    pid_t pid_fils;
    int raison;

    for (int i = 0; i < n; i++) {
        pid = fork();

        switch (pid) {

        case -1:
            raler(1, "fork");

        case 0:
            pid_fils = getpid();
            exit((intmax_t)(pid_fils % 10));
        }
    }

    for (int j = 0; j < n; j++) {
        CHK(pid = wait(&raison));
        printf("pid : %jd\n", (intmax_t)pid);

        if (WIFEXITED(raison))
            printf("exit : %jd\n\n", (intmax_t)(WEXITSTATUS(raison)));
    }

    return 0;
}

// Exercice 6.4

int read_while(int fd) {

    char c;

    int n;
    CHK(n = read(fd, &c, 1));
    printf("%c", c);

    while (n) {
        CHK(n = read(fd, &c, 1));
        printf("%c", c);
    }

    CHK(n);
    return 0;
}

int file_processus(char *path) {

    int fd;
    CHK(fd = open(path, O_RDONLY));

    pid_t pid;
    pid = fork();

    switch (pid) {
    case -1:
        raler(1, "fork");

    case 0:
        read_while(fd);
        exit(0);

    default:
        read_while(fd);
        exit(0);
    }

    close(fd);
    return 0;
}

// Exercice 6.5

int exec_time(char *path) {

    struct timeval begin;
    struct timeval end;
    int d;

    CHK((d = gettimeofday(&begin, NULL)));

    int raison;

    switch (fork()) {

    case -1:
        raler(1, "fork");

    case 0:
        CHK(execlp("ls", "ls", "-l", path, NULL));
        exit(0);

    default:
        CHK(wait(&raison));

        if (WIFEXITED(raison)) {
            if (WEXITSTATUS(raison) > 0)
                raler(0, "bad exit status\n");
        }

        CHK((d = gettimeofday(&end, NULL)));

        double sec = (double)(end.tv_usec - begin.tv_usec) / 1000000 +
                     (double)(end.tv_sec - begin.tv_sec);

        printf("time during ls -l : %ju\n", (uintmax_t)(sec));

        return 0;
    }
}

// Exercice 6.8

int random_pro(size_t n, size_t m) {

    if (m > 255)
        raler(1, "m");

    int *tab = malloc(n * sizeof(int));

    for (size_t i = 0; i < n; i++)
        tab[i] = 1 + rand() % m;

    int raison;

    for (size_t i = 0; i < n; i++) {

        switch (fork()) {

        case -1:
            raler(1, "fork");

        case 0:
            sleep((unsigned)tab[i]);
            exit(tab[i]);
        }
    }

    for (size_t i = 0; i < n; i++) {

        CHK(wait(&raison));

        if (WIFEXITED(raison)) {
            if (WEXITSTATUS(raison) > 0)
                raler(0, "bad exit status\n");
        }
    }

    free(tab);
    return 0;
}

// Exercice 6.9

int bash_cmd(char *user) {

    char buffer[256];

    int n = snprintf(buffer, sizeof(buffer), "^%s", user);
    CHK(n);

    int fd;
    int raison;

    CHK((fd = open("toto", O_RDWR | O_CREAT | O_TRUNC, 0666)));

    switch (fork()) {

    case -1:
        raler(1, "fork");

    case 0:
        CHK(dup2(fd, 1));
        CHK(close(fd));

        execlp("ps", "ps", "eaux", NULL);
        raler(1, "execlp");

    default:
        CHK(wait(&raison));

        if (WIFEXITED(raison)) {
            if (WEXITSTATUS(raison) > 0)
                raler(0, "bad exit status\n");
        }
    }

    switch (fork()) {

    case -1:
        raler(1, "fork");

    case 0:
        CHK(dup2(fd, 0));
        CHK(close(fd));

        execlp("grep", "grep", user, NULL);
        raler(1, "execlp");

    default:
        CHK(wait(&raison));

        if (WIFEXITED(raison)) {
            if (WEXITSTATUS(raison) > 0)
                raler(0, "bad exit status\n");
        }
    }

    CHK(close(fd));
    return 0;
}
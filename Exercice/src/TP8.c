#include "../include/TP8.h"
#define BUFFER_SIZE 4096

// Exercice 8.1
void copier(int fdsrc, int fddst) {

    char buffer[BUFFER_SIZE];
    int n;

    while ((n = read(fdsrc, buffer, BUFFER_SIZE)) > 0) {
        CHK(write(fddst, buffer, n));
    }

    CHK(n);
}

// int main(void) {

//     int tube[2];
//     int status;

//     CHK(pipe(tube));

//     switch (fork()) {

//     case -1:
//         raler(1, "fork");

//     case 0:
//         close(tube[0]);
//         copier(0, tube[1]);
//         close(tube[1]);
//         exit(0);

//     default:
//         close(tube[1]);
//         copier(tube[0], 1);
//         close(tube[0]);

//         CHK(wait(&status));

//         if (WIFEXITED(status)) {
//             if (WEXITSTATUS(status) > 0)
//                 raler(0, "bad exit status\n");
//         }
//     }
// }

// Exercice 8.4

// int main(int argc, char **argv) {

//     if (argc != 2) {

//         fprintf(stderr, "Error on argument\n");
//         exit(1);
//     }

//     char user[BUFFER_SIZE];
//     CHK(snprintf(user, sizeof(argv[1]) + 1, "^%s", argv[1]));

//     int tube1[2];
//     CHK(pipe(tube1));

//     switch (fork()) {

//     case -1:
//         raler(1, "fork");

//     case 0:
//         CHK(close(tube1[0]));
//         CHK(dup2(tube1[1], 1));

//         execlp("ps", "ps", "eaux", NULL);
//         raler(1, "execlp");
//     }

//     CHK(close(tube1[1]));

//     int tube2[2];
//     CHK(pipe(tube2));

//     switch (fork()) {

//     case -1:
//         raler(1, "fork");

//     case 0:
//         CHK(close(tube2[0]));

//         CHK(dup2(tube1[0], 0));
//         CHK(close(tube1[0]));

//         CHK(dup2(tube2[1], 1));
//         CHK(close(tube2[1]));

//         execlp("grep", "grep", user, NULL);
//         raler(1, "execlp");

//     default:
//         CHK(close(tube1[0]));
//         CHK(close(tube2[1]));

//         CHK(dup2(tube2[0], 0));
//         CHK(close(tube2[0]));

//         execlp("wc", "wc", "-l", NULL);

//         raler(1, "execlp");
//     }
// }
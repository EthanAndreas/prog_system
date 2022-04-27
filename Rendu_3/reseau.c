#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXSTA 10
#define BUFFER_SIZE 4096
#define PAYLOAD_SIZE 4
#define FRAME_SIZE 8
#define MAX_LEN 6

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

struct frame_t {

    int destination;
    char payload[4];
};

struct frame_origin_t {

    int destination;
    char payload[4];
    int origin;
};

int main(int argc, char **argv) {

    if (argc != 2) {

        fprintf(stderr, "usage: error on argument\n");
        exit(EXIT_FAILURE);
    }

    int nb_sta = atoi(argv[1]);

    if (nb_sta < 1 || nb_sta > MAXSTA) {

        fprintf(stderr, "usage: number of station is over 10\n");
        exit(EXIT_FAILURE);
    }

    int pipe_table[MAXSTA + 1][2];

    // création du tube commun entre station et commutateur en i = 0
    // et création des tubes privés entre station et commutateur
    for (int i = 0; i < nb_sta + 1; i++)
        CHK(pipe(pipe_table[i]));

    int status;
    int n;

    char payload[PAYLOAD_SIZE + 1];
    char sta_name[5];

    struct frame_t frame;
    struct frame_origin_t frame_o;

    // création des fils liés aux stations
    for (int i = 1; i < nb_sta + 1; i++) {

        switch (fork()) {
            int fd_sta;

        case -1:
            raler(1, "fork");

        case 0:
            CHK(snprintf(sta_name, MAX_LEN, "%s%i", "STA_", i));
            CHK((fd_sta = open(sta_name, O_RDONLY)));

            CHK(close(pipe_table[0][0]));

            for (int j = 1; j < nb_sta + 1; j++) {

                if (i != j)
                    CHK(close(pipe_table[j][0]));

                CHK(close(pipe_table[j][1]));
            }

            // on lit toutes les trames à émettre de la station
            while ((n = read(fd_sta, &frame, sizeof(frame)))) {

                // on écrit la trame dans le tube commun avec la numéro de
                // station de celle qui a émis la trame
                frame_o.destination = frame.destination;
                frame_o.origin = i;
                strncpy(frame_o.payload, frame.payload, PAYLOAD_SIZE);
                CHK(write(pipe_table[0][1], &frame_o, sizeof(frame_o)));
            }

            CHK(close(fd_sta));
            CHK(close(pipe_table[0][1]));

            // on récupère les trames reçues par la station
            while (
                (n = read(pipe_table[i][0], &frame_o, sizeof(frame_o)))) {

                // on ajoute un '\0' afin de pouvoir l'afficher
                strncpy(payload, frame_o.payload, PAYLOAD_SIZE);
                payload[PAYLOAD_SIZE] = '\0';

                fprintf(stdout, "%i - %i - %i - %s\n", i, frame_o.origin,
                        frame_o.destination, payload);
                fflush(stdout);
            }

            CHK(n);
            CHK(close(pipe_table[i][0]));

            exit(0);
        }
    }

    CHK(close(pipe_table[0][1]));

    for (int j = 1; j < nb_sta + 1; j++)
        CHK(close(pipe_table[j][0]));

    // on lit les trames écrites sur le tube commun
    while ((n = read(pipe_table[0][0], &frame_o, sizeof(frame_o)))) {

        // si la destination est valable, on l'envoit à la station
        if (frame_o.destination < nb_sta + 1 && frame_o.destination > 0) {

            // on écrit la trame dans le tube privé
            CHK(write(pipe_table[frame_o.destination][1], &frame_o,
                      sizeof(frame_o)));

        } else {

            // sinon on l'envoit à toutes les stations exceptées celle qui
            // l'a émise
            for (int j = 1; j < nb_sta + 1; j++) {

                if (j != frame_o.origin) {

                    // on écrit la trame dans le tube privé
                    CHK(write(pipe_table[j][1], &frame_o,
                              sizeof(frame_o)));
                }
            }
        }
    }

    CHK(n);

    for (int j = 1; j < nb_sta + 1; j++)
        CHK(close(pipe_table[j][1]));

    CHK(close(pipe_table[0][0]));

    // on attend la terminaison des fils
    CHK(wait(&status));

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) > 0)
            raler(0, "bad exit status\n");
    }

    exit(EXIT_SUCCESS);
}
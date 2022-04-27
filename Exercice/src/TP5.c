#include "../include/TP5.h"

// Exercice 5.4

int copie(char *input, char *output) {

    int d1;
    int d2;
    CHK(d1 = open(input, O_RDONLY));
    CHK(d2 = open(output, O_WRONLY | O_CREAT | O_TRUNC, 066));

    char buffer[BUFFER_SIZE];
    int n;

    while ((n = read(d1, buffer, BUFFER_SIZE)) > 0) {
        CHK(write(d2, buffer, n));
    }

    CHK(n);

    close(d1);
    close(d2);
    return 0;
}

// Exercice 5.5

int get_char(int d) {

    unsigned char c;

    if (read(d, &c, 1) <= 0)
        return MY_EOF;

    else
        return c;
}

// Exercice 5.6

int get_char1(int d) {

    unsigned char c;

    if (read(d, &c, 1) <= 0)
        return MY_EOF;

    else
        return c;
}

// Exercice 5.8

int perm(char *path) {

    CHK(stat(path, &stbuf));

    mode_t type = stbuf.st_mode & S_IFMT;

    switch (type) {

    case S_IFBLK:
        fprintf(stdout, "Type : Block device\n");
        break;

    case S_IFCHR:
        fprintf(stdout, "Type : Character device\n");
        break;

    case S_IFDIR:
        fprintf(stdout, "Type : Directory\n");
        break;

    case S_IFIFO:
        fprintf(stdout, "Type : FIFO\n");
        break;

    case S_IFLNK:
        fprintf(stdout, "Type : Symlink\n");
        break;

    case S_IFREG:
        fprintf(stdout, "Type : Regular file\n");
        break;

    case S_IFSOCK:
        fprintf(stdout, "Type : Socket\n");
        break;
    }

    mode_t perm = stbuf.st_mode & 0777;
    fprintf(stdout, "Permission : %o\n", perm);

    return 0;
}

// Exercice 5.10

int read_dir(char *path) {

    CHK(stat(path, &stbuf));

    if (S_ISREG(stbuf.st_mode)) {

        fprintf(stderr, "The file is not a directory\n");
        return 1;
    }

    DIR *dp;

    if ((dp = opendir(path)) == NULL) {

        fprintf(stderr, "There is an error in the"
                        " opening of the file\n");
        return 1;
    }

    struct dirent *d;

    while ((d = readdir(dp)) != NULL) {

        if ((strcmp(d->d_name, ".") != 0) &&
            (strcmp(d->d_name, "..") != 0))
            fprintf(stdout, "%s\n", d->d_name);

        errno = 0;
    }

    if (errno != 0)
        raler(1, "readdir");

    CHK(closedir(dp));

    return 0;
}

int find_path(char **path, char *str) {
    // lire les fichiers verifier qu'il n'y ait pas le fichier
    // recherche si repertoire relancer la fonction
    // copier le chemin avant snprintf pour garder le chemin précédent

    CHK(stat(path, &stbuf));
    if (S_ISREG(stbuf.st_mode)) {
    }

    int i = 0;

    while (strcmp(d->d_name[i], str) != 0) {

        CHK(stat(path, &stbuf));
        if (S_ISREG(stbuf.st_mode)) {
            int path = snprintf(d->d_name) find_path()
        }
    }

    int which(char *input) {

        char *path = getenv("PATH");
        // snprintf

        int i = 0;
        while (i < size) {
        }

        return 0;
    }

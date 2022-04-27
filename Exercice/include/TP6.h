#include "../include/raler.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Exercice 6.1
int processus(void);

// Exercice 6.2
int n_processus(int n);

// Exercice 6.4
int file_processus(char *path);
int read_while(int fd);

// Exercice 6.5
int exec_time(char *path);

// Exercice 6.8
int random_pro(size_t n, size_t m);

// Exercice 6.9
int bash_cmd(char *user);
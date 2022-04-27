#include "../include/raler.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

void handler_c(int sig);

void handler_unknown(int sig);

void psignal(int s, const char *msg);

unsigned int alarm(unsigned int seconds);

void add_c(int sig);

void end_term(int sig);

void count_inf(void);
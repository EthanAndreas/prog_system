#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define MY_EOF 256
#define BUFFER_SIZE 1024

int copie(char *input, char *output);

int get_char(int d);

int get_char1(int d);

struct stat stbuf;
int perm(char *path);

int read_dir(char *path);

int find_path(char **path);
int which(char *input);
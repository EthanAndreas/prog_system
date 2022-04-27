// Library
#include <stdio.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Macro
#define CHK(op) do { if ((op) == -1) raler (1, #op); } while (0)
#define BUFFER_SIZE 2048

noreturn void raler (int syserr, const char *msg, ...)
{
    va_list ap;

    va_start (ap, msg);
    vfprintf (stderr, msg, ap);
    fprintf (stderr, "\n");
    va_end (ap);

    if (syserr == 1)
        perror ("");

    exit (EXIT_FAILURE);
}

/**
 * @brief The function ** compare ** takes two files
 * in input, then compares characters 1 to 1 and 
 * check their size
 * 
 * @param[in] file1 
 * @param[in] file2 
 */

int compare(char *file1, char *file2){


    // opening files
    int d1, d2;

    CHK(d1 = open(file1,O_RDONLY));
    CHK(d2 = open(file2,O_RDONLY));

    // initialization of the buffer in unsigned char
    // to anticipate the case of a binary file
    unsigned char buffer1[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];

    // read the beginning of the two files
    int n1 = read(d1,buffer1,BUFFER_SIZE);
    int n2 = read(d2,buffer2,BUFFER_SIZE);

    // initialization of the number of bytes and lines browsed
    int byte = 0;
    int line = 1;

    // initialization of files size
    int t1 = 0;
    int t2 = 0;


    // check if one of the two files is empty

    if (n1 == 0 && n2 != 0){

        fprintf(stderr,"EOF on %s which is empty",file1);
        return 1;
    
    }

    if (n1 != 0 && n2 == 0){

        fprintf(stderr,"EOF on %s which is empty",file2);
        return 1;
    }

    // browse files until the end of one of the two, see both

    while (n1 > 0 && n2 > 0){

        int i=0;

        // scan the read bytes 1 to 1

        while (i < n1 && i < n2){

            // count the number of bytes browsed
            byte++;


            // count the number of lines browsed
            if (buffer1[i] == '\n' || buffer2[i] == '\n')

                line++;

            // check the correspondence of the characters
            if (buffer1[i] != buffer2[i]){

                fprintf(stderr,"%s %s differ: "
                "byte %i, line %i\n",file1,file2,byte,line);

                return 1;

            }

            // itérator
            i++;

            // calculate the size of the two files
            t1 = t1 + n1;
            t2 = t2 + n2;

        }
        
        n1 = read(d1,buffer1,BUFFER_SIZE);
        n2 = read(d2,buffer2,BUFFER_SIZE);

    }

    // check if there is no problem on the
    // read system primitive
    CHK(n1);
    CHK(n2);


    // compare the size of the two files

    if (t1 < t2){

        fprintf(stderr,"EOF on %s " 
        "after byte %i, line %i\n",file1,byte,line);

        return 1;

    }

    if (t1 > t2){

        fprintf(stderr,"EOF on %s " 
        "after byte %i, line %i\n",file2,byte,line);

        return 1;

    }

    CHK(close(d1));
    CHK(close(d2));

    return 0;
}

/**
 * @brief Main function, launch the function compare
 * and check the number of argument
 * 
 * @param[in] argc          number of argument
 * @param[in] argv          file 1 and 2 to compare
 */

int main(int argc, char *argv[]){

    if (argc != 3){

        fprintf(stderr,"usage : ./compare file1 file2\n");
        return 1;

    }    

    // we get the result of the function compare
    int ok = compare(argv[1],argv[2]);

    // we check that there was no error
    if (ok == 1) return 1;

    return 0;
}

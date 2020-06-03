#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Write random integers (in binary) to a file with the name given by the command line
 * argument.  This program creates a data file for use by the time_reads program.
 */

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: write_test_file filename\n");
        exit(1);
    }

    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL) {
        perror("fopen");
        exit(1);
    }
    int random_number;
    for (int i = 0; i <100; i ++) {
        random_number = random()%100;
        if (fwrite(&random_number,sizeof(int),1,fp) != 1) {
            fprintf(stderr, "Error: integer can not be written to file\n");
            return 1;
        }
    }
    int error = fclose(fp);
    if (error != 0) {
        fprintf(stderr, "Error: fclose failed\n");
        return 1;
    }
    return 0;
}

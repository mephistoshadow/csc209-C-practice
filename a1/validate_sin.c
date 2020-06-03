#include <stdio.h>
#include <stdlib.h>
int populate_array(int, int *);
int check_sin(int *);

int main(int argc, char **argv) {
    int count = 0;
    if (argc !=2) {
        return 1;
    }
    int *answer = malloc(sizeof(int) * 9);
    while (argv[1][count] !='\0') {
        count = count +1 ;
    }
    if (argv[1][0]=='0'&& count == 9) {
        printf("Invalid SIN\n");
        return 0;
    }
    int go = strtol(argv[1],NULL,10);
    if (populate_array(go,answer)==0) {
        printf("Valid SIN\n");
    }
    if (populate_array(go,answer)==1) {
        printf("Invalid SIN\n");
    }
    

    free(answer);	
  
    return 0;
}

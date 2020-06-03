#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// TODO: Implement a helper named check_permissions that matches the prototype below.
int check_permissions(char * good, char * made) {
    
    for(int i=0; i < 9; i++) {
        if(good[i+1] != made[i] && made[i] != '-') {
            return 1;
        }
    }
    return 0;
}


int main(int argc, char** argv) {
    int count = 0;
    int *check = malloc(sizeof(int)*100);
    char *cool = malloc(sizeof(char)*10000);
    char **name = malloc(sizeof(char*)*9);
    for (int i = 0; i< 9 ; i ++) {
        name[i] = malloc(sizeof(char)*31);
    }

    if (!(argc == 2 || argc == 3)) {
        fprintf(stderr, "USAGE: count_large size [permissions]\n");
        return 1;
    }
    int size = strtol(argv[1],NULL,10);
    while(scanf("%[^\n]%*c", cool) != EOF) {
        char *lol = strtok(cool," ");
        if (lol != NULL) {
            if (strcmp(lol,"total") != 0) {
                for (int i = 0; i< 9; i ++) {
                    name[i] = lol;
                    *check = strtol(name[4],NULL,10);
                    lol = strtok(NULL, " ");
                }
            }
            if (strcmp(name[0],"total") != 0) {
                if (argc ==2 && *check > size){
                    count = count +1;
                }
                if (argc == 3 && *check > size &&  check_permissions(name[0],argv[2])==0) {
                    count = count +1;
                }
            }
        
        }

    }	
  
    printf("%d\n",count);
    free(check);
    free(cool);
    // for(int i = 0;i < 9; i ++) {
    //     free(name[i]);
    // }
    free(name);
    
    return 0;

}

#include <stdio.h>
#include <stdlib.h>

#include "ptree.h"


int main(int argc, char **argv) {
    // TODO: Update error checking and add support for the optional -d flag
    // printf("Usage:\n\tptree [-d N] PID\n");

    // NOTE: This only works if no -d option is provided and does not
    // error check the provided argument or generate_ptree. Fix this!
    struct TreeNode *root = NULL;
    if (argc == 2) {
        if (generate_ptree(&root, strtol(argv[1], NULL, 10))==1) {
            fprintf(stderr, "faild to generate_ptree\n");
            print_ptree(root, 0);
            return 2;
        }
        print_ptree(root, 0);
    }
    if (argc == 4) {
        if (generate_ptree(&root, strtol(argv[3], NULL, 10))==1) {
            fprintf(stderr, "faild to generate_ptree \n");
            print_ptree(root, strtol(argv[2], NULL, 10));
            return 2;
        }
        print_ptree(root, strtol(argv[2], NULL, 10));
    }
    if (argc == 3) {
        if (generate_ptree(&root, strtol(argv[2], NULL, 10))==1) {
            fprintf(stderr, "faild to generate_ptree\n");
            print_ptree(root,0);
            return 2;
        }
        print_ptree(root,0);
    }
    if (argc <2 || argc >4) {
        printf("%d",argc);
        fprintf(stderr, "Usage:\n\tptree [-d N] PID\n");
        return 1;
    }
    
    return 0;
}


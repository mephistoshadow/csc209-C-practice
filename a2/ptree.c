#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
// Add your other system includes here.

#include "ptree.h"

// Defining the constants described in ptree.h
#define LINE_LENGTH 1000
const unsigned int MAX_PATH_LENGTH = 1024;

// If TEST is defined (see the Makefile), will look in the tests 
// directory for PIDs, instead of /proc.
#ifdef TEST
    const char *PROC_ROOT = "tests";
#else
    const char *PROC_ROOT = "/proc";
#endif

int file_openf(FILE **name,char *path) {
    *name = fopen(path,"r");
    if (*name == NULL) {
        return 1;
    }
    return 0;
}


/*
 * Creates a PTree rooted at the process pid.
 * The function returns 0 if the tree was created successfully 
 * and 1 if the tree could not be created or if at least
 * one PID was encountered that could not be found or was not an 
 * executing process.
 */
int generate_ptree(struct TreeNode **root, pid_t pid) {
    // Here's a way to generate a string representing the name of
    // a file to open. Note that it uses the PROC_ROOT variable.
    FILE *cmdfile;
    FILE *childrenfile;
    static int number = 0;
    static int return_value=0;
    number++;
    struct stat exe;
    char *name = malloc(sizeof(char)*1024);
    char *children_name = malloc(sizeof(char)*1024);
    char procfile[MAX_PATH_LENGTH + 1];
    char comandline[MAX_PATH_LENGTH + 1];
    char children[MAX_PATH_LENGTH + 1];
    char *ptr;
    int check_exist = 0;
    int make = 0;
    int count = 0;
    int pid_another =0;
    struct TreeNode *node = malloc(sizeof(struct TreeNode));
    struct TreeNode *temp;
    // create the path for the exe files.
    if (sprintf(procfile,"%s/%d/exe", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename\n");
        return_value = 1;
    }
    // check if it is exist.
    check_exist = lstat(procfile,&exe);
    // create the path for the cmdline files.
    if (sprintf(comandline, "%s/%d/cmdline", PROC_ROOT, pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename\n");
        return_value = 1;
    }
    // create the path for the children files.
    if (sprintf(children, "%s/%d/task/%d/children", PROC_ROOT, pid,pid) < 0) {
        fprintf(stderr, "sprintf failed to produce a filename\n");
        return_value = 1;
    }
    //to see if it can open
    if (file_openf(&cmdfile,comandline) == 1) {
        return_value = 1;
    }
    //to see if it can open
    if (file_openf(&childrenfile,children)==1) {
        return_value = 1;
    }
    // get the name of the node
    fgets(name,LINE_LENGTH+1,cmdfile);
    // get the name of the node 's children
    fgets(children_name,LINE_LENGTH+1,childrenfile);
    make = strlen(children_name);
    //check the size of the children.
    //assgin the node's name and the other thing
    node -> name = name;
    node -> pid =pid;
    //check the exist check if it is successfully.
    if(check_exist < 0) {
          node ->pid = 0;
          perror("lstat");
          return_value = 1;
    }
    node ->child = malloc(sizeof(struct TreeNode));
    if (number ==1 || (*root)->sibling->pid == 0) {
        node -> sibling = NULL;
    }else {
        node -> sibling = (*root)->sibling;
    }
    // if the child is not null to check the values and do the recursive function.
    if (make != 0) {
        ptr = strtok(children_name," ");
        pid_another = strtol(ptr,NULL,10);
        node ->child -> pid = pid_another;
        *root = node;
        //assign the root to node.
        (*root) -> child ->sibling = malloc(sizeof(struct TreeNode));
        temp = &(*(*root) -> child ->sibling);
        //let temp to equals the root child's sibling
        while(ptr != NULL) {
            count = count +1;
            ptr = strtok(NULL," ");
            // check the child's sibling if is exist.
            if (ptr != NULL) {
                pid_another = strtol(ptr,NULL,10);
                (*temp).pid = pid_another;
                temp-> sibling = malloc(sizeof (struct TreeNode));
                temp = temp -> sibling;
            }
        }
        //do the recursive function to check the child and grandchild.
        generate_ptree(&(*root)->child,(*root)->child->pid);
        if ((*root)->sibling!= NULL) {
            // to do the recursive function to do the sibling and draw.
            generate_ptree(&(*root)->sibling,(*root)->sibling->pid);
            
        }
        // close the files.
         if (fclose(cmdfile)!=0) {
             fprintf(stderr,"faild to close the file\n");
            return_value = 1;
        }
        // close the files.
        if (fclose(childrenfile)!=0) {
            fprintf(stderr,"faild to close the file\n");
            return_value = 1;
        }
    }
    //if the node doesn't has the child, do the following operations and check if it has sibling.
    if (make == 0) {
        node -> child = NULL;
        *root = node;
        //close the files/
        if (fclose(childrenfile)!=0) {
            fprintf(stderr,"faild to close the file\n");
            return_value = 1;
        }
         if ((*root)->sibling!= NULL) {
            generate_ptree(&(*root)->sibling,(*root)->sibling->pid);
            
        }

    }
   
    return return_value;
}

/*
 * Prints the TreeNodes encountered on a preorder traversal of an PTree
 * to a specified maximum depth. If the maximum depth is 0, then the 
 * entire tree is printed.
 */
void print_ptree(struct TreeNode *root, int max_depth) {
    // Here's a way to keep track of the depth (in the tree) you're at
    // and print 2 * that many spaces at the beginning of the line.
    if ((*root).pid !=0) {
    
        static int depth = 0;
        int ant = 0;
        if (max_depth == 0) {
            if (strcmp((*root).name,"")!=0) {
                printf("%*s", depth * 2, "");
                printf("%d",(*root).pid);
                printf("%c",':');
                printf("%c",' ');
                printf("%s",(*root).name);
                printf("\n");
            }else {
                printf("%*s", depth * 2, "");
                printf("%d",(*root).pid);
                printf("\n");
            }
        
            if ((*root).child != NULL) {
                ant = depth;
                depth = depth +1;
                print_ptree((*root).child,max_depth);
                depth = ant;
            }

            if ((*root).sibling != NULL) {
                print_ptree((*root).sibling,max_depth);
            }

         }
    

        if (max_depth != 0) {
            if (strcmp((*root).name,"")!=0) {
                printf("%*s", depth * 2, "");
                printf("%d",(*root).pid);
                printf("%c",':');
                printf("%c",' ');
                printf("%s",(*root).name);
                printf("\n");
            }else {
                printf("%*s", depth * 2, "");
                printf("%d",(*root).pid);
                printf("\n");
            }
            if ((*root).child != NULL) {
                ant = depth;
                depth = depth + 1;
                if (depth <= max_depth) {
                    print_ptree((*root).child,max_depth);
                }
                depth = ant;
           
            }
            if ((*root).sibling != NULL) {
                print_ptree((*root).sibling,max_depth);
            }
        
        }
    }
    



}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256
#define MAX_PASSWORD 10

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];
  int fd[2];
  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }

  if (pipe(fd)==-1) {
    perror("pipe");
    exit(1);
  }
  int check = fork();
    if (check == -1) {
        perror("fork failed");
         exit(1);
    }
    else if (check>0) {
      if ((close(fd[0])) == -1) {
            perror("close");
            exit(1);
        }
        if(write(fd[1],user_id,MAX_PASSWORD) == -1) {
          perror("write");
          exit(1);
        }

        if(write(fd[1],password,MAX_PASSWORD) == -1) {
          perror("write");
          exit(1);
        }
        if ((close(fd[1])) == -1) {
            perror("close");
              exit(1);
        }
    }else {
      if ((dup2(fd[0], STDIN_FILENO)) == -1) {
          perror("dup2");
          exit(1);
      }
      if(close(fd[1])==-1) {
        perror("close");
          exit(1);
      }
      if(close(fd[0]) == -1) {
        perror("clpse");
         exit(1);
      }
      int i = execl("./validate","validate",NULL);
      exit(i);
    }

    int status;
    if(wait(&status) == -1) {
      perror("wait");
    }
    if (WIFEXITED(status)) {
        const int es = WEXITSTATUS(status);
        if (es == 0) {
          printf("%s",SUCCESS);
        }
        if (es == 1) {
           exit(1);
        }
        if (es == 2) {
          printf("%s",INVALID);
        }
         if (es == 3) {
          printf("%s",NO_USER);
        }
    }


  return 0;
}

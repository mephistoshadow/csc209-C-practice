#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include "filter.h"
#include <errno.h>

//this is used to print that number can be factor with two prime
void print_message_can(int input,int factor_one,int factor_two) {
    printf("%d %d %d\n",input,factor_one,factor_two);
}
// this is used to print that the number is a prime number
void print_message_prime(int input) {
    printf("%d is prime\n",input);
}
// this is used to print that the number can not be factored with two primes.
void print_message_not(int input) {
    printf("%d is not the product of two primes\n",input);
}
// this is help us check whether the number is perfect square.
void perfect_square(int input,int check_number,int *check_perfect_square) {
    if (check_number * check_number == input){
        (*check_perfect_square)++;
    }
}
// this is used to help us do check read action
void Read(int check) {
    if (check == -1) {
        perror("read");
    }
}
// this is help us to check pipe thing.
void Pipe(int check) {
    if (check == -1) {
        perror("pipe");
    }
}
// this is help us to do check write thing.
void Write(int check) {
    if (check == -1) {
        if (errno == EPIPE) {

        }else {
            perror("write");
        }
    }
}
// this is used to help us do the close thing.
void Close(int check) {
    if (check == -1) {
        perror("close");
    }
}
void print_filenumber(int filter) {
    printf("Number of filters = %d\n", filter);
}

int main(int argc, char **argv) {
     if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
          perror("signal");
          exit(1);
    }
    // check too few and too many argumant.
    if (argc != 2) {
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    char *check_input = NULL;
    // this is the input number 
    int input = strtol(argv[1],&check_input,10);
    // check the number less than 2
    if (input < 2) {
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    // handle the situation like "123cbd" print usage error.
    if (strcmp(check_input,"")!=0) {
        fprintf(stderr, "Usage:\n\tpfact n\n");
        exit(1);
    }
    // this is the input number's square root.
    int get_sqrt = sqrt(input);
    // this is used to generate the pipe that we use to pass values.
    int fd[get_sqrt][2];
    // this is the facter array that store the factors of input.
    int facters[2];
    // initialize the array
    facters[0] = 0;
    // initialize the array
    facters[1] = 0;
    // this is used to count the factor number of the input.
    int count_factor = 0;
    // this is used to count the filters
    int count_filter = 0;
    // this is used to check the for status.
    int check_fork = 0;
    // this is used to get the filter number m
    int first_read_value = 0;
    // this is used to store the value that we write to it.
    int write_value =0;
    // this variable is hlep us to determine whether the number is perfect square.
    int check_perfect_square = 0;
    // check the number is 2 or 3
    if (input == 2 || input == 3) {
        int filter_number =0;
        print_message_prime(input);
        print_filenumber(filter_number);
    }
    // here the the solution that we solve for number large than 3.
    else{
        // this is used to generate the filter and use to determine the facter.
        for(int i = 0; i < get_sqrt; i++) {
            if (i == 0) {
                Pipe(pipe(fd[i]));
            }
            if (i == 0) {
                check_fork = fork();
            }
            if (check_fork == -1) {
                perror("fork");
            }
            // this is master process.
            if (check_fork > 0 && i == 0) {
                close(close(fd[0][0]));
                // here is write from 2 start and write two times. one for check
                // the other for pas
                int first =2;
                Write(write(fd[0][1],&first,sizeof(int)));
                for (int j=2; j <= input; j ++) {
                    Write(write(fd[0][1],&j,sizeof(int)));
                }
                // collect the filter number in total.
                Close(close(fd[0][1]));
                int wstatus;
                   wait(&wstatus);
                   count_filter = WEXITSTATUS(wstatus);
                   print_filenumber(count_filter);
                exit(0);
            }
            // this is the next series parents that are not master process.
            if (check_fork > 0 && i != 0) {
                Close(close(fd[i][0]));
                Read(read(fd[i-1][0],&first_read_value,sizeof(int)));
                Write(write(fd[i][1],&first_read_value,sizeof(int)));
                // filter the number
                if (first_read_value < get_sqrt) {
                    if (filter(first_read_value,fd[i-1][0],fd[i][1]) == 1) {
                        fprintf(stderr,"can not success do the filter operation");
                        exit(1);
                    }
                }
                Close(close(fd[i-1][0]));
                Close(close(fd[i][1]));
                // check the if the input is perfect suqare.
                perfect_square(input,first_read_value,&check_perfect_square);
                if (first_read_value < get_sqrt) {
                    int wstatus;
                       wait(&wstatus);
                       count_filter = WEXITSTATUS(wstatus);
                }
                // if the number is perect square do not add one exit code
                if (check_perfect_square == 1) {
                    exit(count_filter);
                }
                // check some cases that will exit one more filter.
                // checked for number of process that I v created.
                if(first_read_value * first_read_value > input) {
                    exit(count_filter);
                }
                // check whether the input has two prime factors that less than square root
                // and stop count filter number +1 to match the correct number of
                // process that we have created.
                if (facters[0] != 0 && facters[0] != first_read_value 
                    && input % first_read_value == 0) {
                    exit(count_filter);
                }
                exit(1+count_filter);
            }
            // here is to imple ment the child process. for another child
            // in the pipe line
            // and check the terminate condition
            if (check_fork == 0){
                if (i > 0) {
                    Read(read(fd[i][0],&write_value,sizeof(int)));
                    // count the factor.. if the m is can be divided by input.
                    if (input % write_value == 0) {
                        facters[count_factor] = write_value;
                        count_factor ++;
                    }
                    // check the counter factor number and print prime srate information
                    if (count_factor == 0 && write_value >= get_sqrt) {
                        print_message_prime(input);
                        exit(0);
                    }
                    // check if number of input's factor is 2, and if it is print this 
                    // is not the product of two prime.
                    //printf("%d",count_factor);
                    if (count_factor == 2) {
                        print_message_not(input);
                        exit(0);
                    }
                    // check the if it can be factor. But in these number make additionally
                    // check the if it is a cubic of a prime.
                    // also distinguish the input is perfect square number.
                    if (count_factor == 1 && write_value >= get_sqrt) {
                        int another_factor = input/facters[0];
                        if (another_factor % facters[0] == 0 && another_factor != facters[0]) {
                            check_perfect_square ++;
                            print_message_not(input);
                            exit (0);
                        }else {
                            print_message_can(input,facters[0],input/facters[0]);
                            exit (0);
                        }
                    }
                }
                Close(close(fd[i][1]));
                Pipe(pipe(fd[i+1]));
                check_fork = fork();
            }
        }
    }
    return 0;
}









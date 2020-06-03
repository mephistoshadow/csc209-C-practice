#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <errno.h>
int filter(int m, int read_fd, int write_fd) {
	int write_value=0;
	while(read(read_fd,&write_value,sizeof(int))>0) {
		if (write_value % m != 0) {
			if (write(write_fd,&write_value,sizeof(int)) == -1) {
				if (errno == EPIPE) {

				}else {
					perror("write");
					return 1;
				}
			}
		}
	}
	return 0; 
}

#include <stdlib.h>
// TODO: Implement populate_array
/*
 * Convert a 9 digit int to a 9 element int array.
 */

int check_sin(int *sin_array) {
    int sum = 0;
    int *made = malloc(sizeof(int) * 9);
        // printf("%d",sin_array[0]);
    for (int i=0;i < 9; i++) {
        if (i%2!=0) {
            made[i] = sin_array[i] *2;
            if (made[i]/10 > 0) {
                    made[i] = made[i]/10 + made[i]%10;
                        // printf("%d\n",made[i]);
                }
        }
        else if (i%2 == 0) {
            made[i] = sin_array[i] *1;
        }
    }

    for (int i =0; i < 9; i ++) {
            sum = sum + made[i];
		 
    }

    if (sum %10 == 0) {
        return 0;
    }
    return 1;
}

int populate_array(int sin, int *sin_array) {
    int count = 1;
    int sec = sin;
	// int revers = 0;
    int * good = malloc(sizeof(int)*9);
    sin_array = malloc(sizeof(int) * 9);
	while(sin > 9) {
            sin = sin / 10;
            count = count +1;
		// printf("%d",count);
    } 
    if (count != 9) {
            return 1;
    }

    for (int i = count - 1; i >= 0; i --) {
            good[i] = sec % 10;
            sec = sec / 10;
		// printf("%d",good[i]);
    }
    for (int i = 0; i< 9; i ++ ) {
            sin_array[i] = good[i];
            // printf("%d",sin_array[i]);
		
    }
	// printf("%d",sin_array[1]);
    if (check_sin(sin_array)==0) {
        return 0;
            //printf("Valid SIN\n");
    }
    else {
            //printf("Invalid SIN\n");
        return 1;
    }
		

    return 0;
}

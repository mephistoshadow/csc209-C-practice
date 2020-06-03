#include <stdio.h>


int main() {
    char phone[11];
    int integer_one;
    scanf("%s %d",phone,&integer_one);
    if(integer_one == 0) {
            printf("%s\n",phone);
            return 0;
    }
    if(integer_one  >= 1 && integer_one  <= 9) {
            printf("%c\n",phone[integer_one]);
            return 0;
    }
    if(integer_one  < 0 || integer_one > 9) {
            printf("ERROR\n");
            return 1;
    }
    
}
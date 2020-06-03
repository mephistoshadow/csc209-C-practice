#include <stdio.h>
int main(){
    char phone[11];
    int integer_one;
    int count = 0;
    scanf("%s",phone);
    while(scanf("%d",&integer_one) != EOF) {
        if(integer_one == 0) {
            printf("%s\n",phone);
        }
        if(integer_one  >= 1 && integer_one  <= 9) {
            printf("%c\n",phone[integer_one]);
        }
        if(integer_one  < 0 || integer_one > 9) {
            printf("ERROR\n");
            count = count +1;
        }
    }
    if(count == 0) {
        return 0;
    }
    if(count != 0) {
        //printf("not good");
        return 1;
    }
}
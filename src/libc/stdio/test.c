#include <stdio.h>

char *convert(unsigned int num, int base) {
    static char value[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;
    int a = 0;
    int i = 48;
    ptr = &buffer[49];
    *ptr = '\0';
    
    while(num != 0) {
        a = num % base;
        if (a < 10) {
            *--ptr = a + 48;
            //buffer[i] = a + 48;
            //i--;
        }
        else {
            *--ptr = a + 55;
            //buffer[i] = a + 55;
            //i--;
        }


    return ptr;
    }
}

int main() {
    char *w = convert(8, 10);
    printf("val: %d\n", *w);
    return 0;
}

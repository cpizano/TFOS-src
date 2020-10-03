// lab_001a.c

#include <stdio.h>

int main() {
    int number;
    printf("enter an integer: ");
    if (scanf("%d", &number) != 1)
        return 1;  
    printf("twice that: %d", number * 2);
    return 0;
}

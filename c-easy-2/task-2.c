#include <stdio.h>
typedef struct PerInfo {
    char   name[10];   
    char   gender;     
    int    age;        
    double height;     
} PerInfo;

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void) {
    int x,y;
    swap(&x, &y);
    printf("%llu",sizeof(PerInfo));
    return 0;
}
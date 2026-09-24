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
    int x=3,y=5;
    swap(&x, &y);
    printf("%d %d\n",x,y);
    printf("%llu\n",sizeof(PerInfo));
    return 0;
}

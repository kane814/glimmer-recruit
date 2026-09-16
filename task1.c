#include <stdio.h>
int exponentiation(int);
int calc(int,int,int);
void compare(int,int,int);
int main(){
    int i=0,x=0;
    int age[100];
    char a[100][20];
    do{
        printf("请输入年龄\n");
        scanf("%d",&age[i]);           //此处不能有换行符，会卡住
        printf("请输入姓名\n");
        scanf("%19s",a[i]);
        printf("年龄是：%d,姓名是：%s\n",age[i],a[i]);
        i++;
        loop:printf("继续请按1,退出请按0\n");
        scanf("%d",&x);
        if(x!=1&&x!=0){
            goto loop;
        }
    }while(x==1&&i<100);
    printf("%d",i);
    return 0;
}
int exponentiation(int x){
    
    x=x*x;
    return x;
}
int calc(int x1,int x2,int x3){
  int p1 = (x1 + x2 + x3) / 3;
  int f1 = ((p1 - x1) * (p1 - x1) + (p1 - x2) * (p1 - x2) + (p1 - x3) * (p1 - x3)) / 3;
  int zh1 = 3 * p1 - f1 / 3;
  return zh1;
}
void compare(int zh1,int zh2,int zh3){
    if (zh1 >= zh2 && zh2 >= zh3) {
      printf("小明 > 小强 > 小林");
  } else if (zh1 >= zh3 && zh3 >= zh2) {
      printf("小明 > 小林 > 小强");
  } else if (zh2 >= zh1 && zh1 >= zh3) {
      printf("小强 > 小明 > 小林");
  } else if (zh2 >= zh3 && zh3 >= zh1) {
      printf("小强 > 小林 > 小明");
  } else if (zh3 >= zh1 && zh1 >= zh2) {
      printf("小林 > 小明 > 小强");
  } else { // zh3 >= zh2 && zh2 >= zh1
      printf("小林 > 小强 > 小明");
}

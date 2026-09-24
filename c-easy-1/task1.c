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
    printf("共完成了%d次\n",i);

    //三项成绩，顺手把上面写的函数用起来
    int x1,x2,x3,y1,y2,y3,z1,z2,z3;
    printf("请输入小明的三项成绩（顺序为A B C,以一个空格为间隔）：");
    scanf("%d %d %d",&x1,&x2,&x3);
    printf("请输入小强的三项成绩（顺序为A B C,以一个空格为间隔）：");
    scanf("%d %d %d",&y1,&y2,&y3);
    printf("请输入小林的三项成绩（顺序为A B C,以一个空格为间隔）：");
    scanf("%d %d %d",&z1,&z2,&z3);

    int zh1=calc(x1,x2,x3);
    int zh2=calc(y1,y2,y3);
    int zh3=calc(z1,z2,z3);
    printf("小明%d 小强%d 小林%d\n",zh1,zh2,zh3);
    compare(zh1,zh2,zh3);
    printf("\n");
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
}

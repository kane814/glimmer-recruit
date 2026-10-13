#include <stdio.h>
#include<string.h>
int dfs(int,int,int,int,int);
int main(){
printf("%d",dfs(5,0,0,0,0));
}
int counter=0;
int dfs(int N,int row,int cols,int diag1,int diag2){
    int available=((1<<N)-1)&~(cols|diag1|diag2);
    if(row==N){
        counter++;
    return counter;
}
    if(available==0){
        return counter;
}
    while(available){
    int pos=available&(-available);
    available&=(~pos);
    dfs(N,row+1,cols|pos,(diag1|pos)<<1,(diag2|pos)>>1);   
    }
    return counter;
}
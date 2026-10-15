#include <stdio.h>
int main(){
    char count1[9];
    char count2[9];
    scanf("%8s %8s",count1,count2);

    unsigned char a=0,b=0;
    for(int i=1;i<7;i++){
        a|=count1[i]-'0';
        a<<=1;;
        b|=count2[i]-'0';
        b<<=1;
    }
    unsigned char exponentA=a>>3;
    unsigned char exponentB=b>>3;
    unsigned char significandA=8|(a&7);
    unsigned char significandB=8|(b&7);
    unsigned char exponent=0; 
    int r;
if (exponentA >= exponentB) {
    r = exponentA - exponentB;
    significandB >>= r;
    exponent = exponentA;
} else {
    r = exponentB - exponentA;
    significandA >>= r;
    exponent = exponentB;
}
    unsigned int t=significandA+significandB;
    if(t>=16){
        t>>=1;
        exponent+=1;
    }
    unsigned char fraction=t&7;
    unsigned char result=0;
    result|=(exponent<<3)|fraction;
for (int i = 7; i >= 0; i--) {
    printf("%u", (result >> i) & 1u);
}
printf("\n");
    return 0;
}
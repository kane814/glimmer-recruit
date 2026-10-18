#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 这是一个隐藏的后门函数，正常流程中永远不会被调用
void grant_access() {
    printf("[System] Access Granted. Releasing Prisoner 'Cool Cipher'...\n");
    system("/bin/sh");  // 开启最高权限的 Shell
}

void verify_identity() {
    char password[16];  // 分配了 16 字节的缓冲区用于存放密码

    printf("Enter Admin Password: ");

    // 致命弱点：gets() 函数不检查用户输入的长度！
    gets(password);

    if (strcmp(password, "Quantum_Neuro_Sec_999") == 0) {
        printf("[System] Welcome, Admin.\n");
    } else {
        printf("[System] Access Denied. Commencing Purge...\n");
    }
}

int main() {
    verify_identity();
    return 0;
}
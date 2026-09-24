#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct a{
    int data;         //结构体里不能初始化
    struct a *next;
}Node;

Node* create(int);
void prepend(Node*,Node**);
void append(Node*,Node**);
void print(Node*);
Node* find(int n,Node*);
bool delete(Node*,int);
void reverse(Node**);

int main(){
    Node* Head=create(0);
    prepend(create(1),&Head);
    prepend(create(2),&Head);
    append(create(3),&Head);
    print(Head);
    find(1,Head);
    printf("\n");
    if(delete(Head,2))printf("删除成功\n");
    print(Head);
    reverse(&Head);
    print(Head);
    return 0;
}
//创建一个装value的节点
Node* create(int value){
    Node* p=malloc(sizeof(Node));
    if(p==NULL)return NULL;
    p->data=value;
    p->next=NULL;
    return p;
}
void prepend(Node* s,Node**Head){
    s->next=*Head;
    *Head=s;
}
void append(Node*s,Node**Head){
    s->next=NULL;
    Node *temp=*Head;
    while (temp->next!=NULL)
    {
        temp=temp->next;         //找出末尾
    }
    temp->next=s;
}
//打印，不改头指针
void print(Node* head){
    Node* temp=head;
    while(temp!=NULL){
        printf("%d ",temp->data);
        temp=temp->next;
    }
    printf("\n");
}
Node* find(int n, Node* head){
    int i = 0;
    Node* temp = head;
    while (temp != NULL && temp->data != n)
    {
        i++;
        temp = temp->next;
    }
    if (temp == NULL){
        printf("false");
        return NULL;
    }
    else{
        printf("%d", i);        //距头节点的距离
        return temp;
    }
}
bool delete(Node*head,int n)    //其实可以做个删data值的函数，更实用
{
    if (head == NULL || head->next == NULL)   // 空链表 / 只有虚拟头节点
        return false;
    Node* a=head;
    Node* b=head->next;
    int i=1;
    while(b!=NULL&&i<n)//防止太大了
    {
        a=b;
        b=b->next;
        i++;
    }
    if(b==NULL)
        return false;
    a->next=b->next;
    free(b);
    return true;
}
void reverse(Node**head){
    Node*a=*head;
    Node*b=NULL;
    while(a!=NULL){
        Node*p=a->next;
        a->next=b;
        b=a;
        a=p;
    }
    *head=b;        //头指针指向新的第一个节点
}

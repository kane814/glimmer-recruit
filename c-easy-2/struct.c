#include <stdio.h>
#include <stdbool.h>

typedef struct a{
    int data;         //结构体里不能初始化
    struct a *next;
}Node;

void prepend(Node*,Node**);
void append(Node*,Node**);
Node* find(int n,Node*);
bool delete(Node*,int);
void reverse(Node*);

int main(){
    Node a;
    a.data=0;
    a.next=NULL;
    Node* Head=&a;
    return 0;
}
void prepend(Node* s,Node**Head){
    s->next=(*Head)->next;
    *Head=s;
}
void append(Node*s,Node**Head){
    s->next=NULL;
    if ((*Head)->next == NULL){          // 空链表，防一下吧
        (*Head)->next = s;
        return;
    }
    Node *temp=Head;
    while (temp->next!=NULL)
    {
        temp=temp->next;         //找出末尾,但函数里要初始化好
    }
    temp->next=s;
                                //防范一下吧，虽然理论上应初始化过了
}
Node* find(int n, Node* head){
    int i = 0;
    if (head->next == NULL){          // 空链表
        printf("NULL");
        return NULL;
    }
    i++;
    Node* temp = head;
    while (temp->next != NULL && temp->next->data != n)
    {
        i++;
        temp = temp->next;
    }
    // 循环退出只有两种可能：找到 或 走到末尾
    if (temp->next == NULL){          
        printf("false");
        return NULL;
    }
    else{                            
        printf("%d", i);
        return temp->next;
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
    return true;
}
void reverse(Node*head){
    Node*a=head->next;  //这是真实头部
    Node*b=NULL;
    while(a!=NULL){
        Node*p=a->next;
        a->next=b;
        b=a;
        a=p;
    }
    head->next=b;
}
#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    int data;                       // 节点存储的数据
    struct TreeNode *left;          // 左子树指针
    struct TreeNode *right;         // 右子树指针
} TreeNode;

TreeNode* create_node(int value){
    TreeNode* node = malloc(sizeof(TreeNode));
    if (node == NULL) {
        return NULL;
    }
    node->data=value;
    node->left=NULL;
    node->right=NULL;
    return node;
}
void front(TreeNode*);
void mid(TreeNode*);
void back(TreeNode*);
int depth(TreeNode *root, int current_depth, int max_depth);



typedef struct Stack {
    TreeNode **arr;
    int top;
    int capacity;
} Stack;
Stack *createStack(int capacity) {
    Stack *stack = malloc(sizeof(Stack));    //做前期准备
    stack->arr = malloc(sizeof(TreeNode *) * capacity);//
    stack->top = -1;
    stack->capacity = capacity;
    return stack;
}
int isEmpty(Stack *stack) {
    return stack->top == -1;          //防止前面函数忘记调用，导致stack未初始化
}
void push(Stack *stack, TreeNode *node) {          //压栈终于来了！第一次带代码里看到这个！
    if (stack->top == stack->capacity - 1) {       //这是指没有内存空间吗？因为这是int类型，所以只能起模拟作用
        return;
    }
    stack->arr[++stack->top] = node;               //开始赋值。运用数组，通过++stack->top模拟栈帧的增加，然后写入node
}
TreeNode *pop(Stack *stack) {                      //同理这就是模拟弹栈了（关键是用“--”方便后期覆写），但实际意义有点不同，似乎也没起到free的效果。可惜。
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->arr[stack->top--];
}

void preorderTraversal(TreeNode *root){           //这个覆写的逻辑感觉真正把递归区分出来了
     Stack* Stack=createStack(100);
     push(Stack,root);
     while(!isEmpty(Stack)){
        TreeNode* node=pop(Stack);                //巧妙，往上走一格，若left压栈失败了,并且用stack中的数组整体操作也很不错
        printf("%d",node->data);
        if(node->right!=NULL){
            push(Stack,node->right);
        }
        if (node->left!=NULL)
        {
            push(Stack,node->left);
        }                                           
     }
}





int main(){
    TreeNode* x[7];
    x[0]=create_node(-1);
    int i;
    for(i=1;i<7;i++){
        x[i]=create_node(i);
    }
    for(i=1;2*i+1<7;i++){
        x[i]->left=x[2*i];
        x[i]->right=x[2*i+1];
    }
    x[3]->left=x[6];
    front(x[1]);
    printf("\n");
    mid(x[1]);
    printf("\n");
    printf("%d",depth(x[1],0,0));
    preorderTraversal(x[1]);

    return 0;
}
void front(TreeNode*p){
    if (p==NULL){return;}
    printf("%d",p->data);
    front(p->left);
    front(p->right);
}
void mid(TreeNode*p){
    if(p==NULL){return;}
    mid(p->left);
    //front(p->left);         
    //太神秘了，这都可以,245163。其实是从调用这个函数开始遵循前序的原则
    printf("%d",p->data);
    mid(p->right);
}
void back(TreeNode*p){
    if(p==NULL){return;}
    back(p->left);
    back(p->right);
    printf("%d",p->data);
}
int depth(TreeNode *root, int current_depth, int max_depth){
    if(root==NULL){
        if(current_depth>max_depth){max_depth=current_depth;}
        return max_depth;
    }
    max_depth=depth(root->left,current_depth+1,max_depth);           //刚开始写错了，忘记接受值了，在那运行，但实际上这两个递归并没有起到作用
    max_depth=depth(root->right,current_depth+1,max_depth);
    return max_depth; //保证除了最后一层外也能正常传播
}
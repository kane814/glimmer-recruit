#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_TREE_SIZE 100

typedef struct{
    int data;
    bool used;
}SeqTreeNode;
typedef struct {
   SeqTreeNode nodes[MAX_TREE_SIZE];
   int size;
}SeqBiTree;

void init_tree(SeqBiTree *tree){
    int i;
    if(tree==NULL)return;

    for(i=0;i<MAX_TREE_SIZE;i++){
        tree->nodes[i].data=0;
        tree->nodes[i].used=false;
    }
    tree->size=MAX_TREE_SIZE;
}
bool set_root(SeqBiTree *tree, int value){
    if(tree==NULL)return false;
    tree->nodes[1].data=value;
    tree->nodes[1].used=true;
    return true;
}
bool set_left_child(SeqBiTree* tree,int parent_node,int value){
    int leftchild_node=2*parent_node;
    if(leftchild_node>=MAX_TREE_SIZE)return false;
    tree->nodes[leftchild_node].data=value;
    tree->nodes[leftchild_node].used=true;
    return true;
}
bool set_right_child(SeqBiTree* tree,int parent_node,int value){
    int rightchild_node=2*parent_node+1;
    if(rightchild_node>=MAX_TREE_SIZE)return false;
    tree->nodes[rightchild_node].data=value;
    tree->nodes[rightchild_node].used=true;
    return true;
}
void level_order(SeqBiTree*tree){
    int i;
    int n=1;
    for(i=1;i<MAX_TREE_SIZE;i++){
        if(tree->nodes[i].used==false){
            printf("-1");
        }
        else{
        printf("%d",tree->nodes[i].data);
        }
        if(i==n){
            printf("\n");
            n=n*2+1;
        }
    }
}
int main(){
    SeqBiTree tree;
    init_tree(&tree);
    set_root(&tree, 1);            
    set_left_child(&tree, 1, 2);   
    set_right_child(&tree, 1, 3);  

    set_left_child(&tree, 2, 4);   
    set_right_child(&tree, 2, 5);  
    set_left_child(&tree, 3, 6);   
    set_right_child(&tree, 3, 7);  
    level_order(&tree);
    return 0;
}
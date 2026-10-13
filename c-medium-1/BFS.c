#include <stdio.h>
#include <stdbool.h>

// 定义简单队列结构用于 BFS
typedef struct {
    int pos;   // 当前节点编号 (0~15)
    int dist;  // 到达当前节点的最短步数
} Node;

int minStepsToCheese(int walls) {
    int start = 0;
    int target = 15;

    // 如果起点或终点本身是墙，直接不可达
    if ((walls & (1 << start)) || (walls & (1 << target))) {
        return -1;
    }

    // BFS 队列与访问位图
    Node queue[16];
    int front = 0, rear = 0;
    int visited = 0;

    // 起点入队并标记已访问 (请使用位运算)
    queue[rear++] = (Node){start, 0};
    visited |= (1 << start);

    // 上、下、左、右四个方向的节点偏移量
    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    //请在TO DO 和END OF TO DO 行之间补全代码：
    while(front < rear)
{
    Node a = queue[front++];
    if(a.pos == target)
        return a.dist;
    int row = a.pos/4;
    int col = a.pos%4;
    for(int i=0;i<4;i++)
    {
        int nr=row+dr[i];
        int nc=col+dc[i];
        if(nr<0 || nr>=4 || nc<0 || nc>=4)
            continue;
        int next = nr*4+nc;
        int test = 1<<next;
        if(walls & test)
            continue;
        if(visited & test)
            continue;
        visited |= test;
        queue[rear++] =
            (Node){next,a.dist+1};
    }
}
    //END OF TO DO
    return -1; // 无法到达
}

int main() {
    int walls = (1 << 5) | (1 << 10); // 5号和10号格子是墙
    int steps = minStepsToCheese(walls);
    printf("Minimum steps: %d\n", steps); // 应输出 6
    return 0;
}
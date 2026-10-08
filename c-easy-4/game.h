// game.h —— 常量、类型定义、游戏逻辑的对外接口
#pragma once   // 防止同一个头文件被重复包含（拆成多文件后必须加）

/* ---------------- 常量 ---------------- */
#define BOARD_SIZE  9      // 棋盘边长：9 × 9
#define WIN_LENGTH  5      // 连成几子算赢（step3 会用到这个）
#define EMPTY_CELL  '.'    // 空格的表示。用 '.' 不用 ' '，一眼能看出"这格是空的"

/* ---------------- 游戏状态 ---------------- */
typedef enum {
    GAME_RUNNING,   // 游戏进行中
    GAME_X_WINS,    // X 胜出
    GAME_O_WINS,    // O 胜出
    GAME_DRAW       // 平局（81 格填满且无人获胜）
} GameStatus;

/* ---------------- 落子结果 ---------------- */
typedef enum {
    MOVE_ACCEPT,         // 可以下，而且已经下好了
    MOVE_OUT_OF_BOUNDS,  // 位置在棋盘外
    MOVE_OCCUPIED,       // 位置已经被占了
    MOVE_GAME_OVER       // 游戏已经结束，不再接受落子
} MoveResult;

/* ---------------- 一局游戏的全部数据 ---------------- */
typedef struct {
    char cells[BOARD_SIZE][BOARD_SIZE]; // 棋盘，EMPTY_CELL 表示空
    char player[2];                     // player[0] 先手，player[1] 后手
    char current_player;                // 当前轮到谁落子
    int  move_count;                    // 有效步数（平局判定要用）
    GameStatus status;                  // 游戏状态
} Game;

extern Game game;   // 全局唯一的一局游戏，定义在 game.c

/* ---------------- 游戏逻辑 ---------------- */
void init_game(void);                  // 初始化：清盘、X 先手、状态置为进行中
MoveResult place(int row, int col);    // 在 (row, col) 落一子
int check_win(int row,int col);
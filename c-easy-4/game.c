// game.c —— 游戏逻辑：初始化棋盘、落子
#include "game.h"

Game game;   // 全局唯一的一局游戏（定义在这里，game.h 里只是 extern 声明）

int check_win(int row,int col){
    int ro=row;int co=col;
    int counter=1;
    int dirs[4][2]={
        {1,0},{1,1},{1,-1},{0,1}
    };
    for(int i=0;i<4;i++){
        int cx=dirs[i][0];
        int cy=dirs[i][1];
        row=ro;col=co;
        row+=cx;col+=cy;       
        while(row>=0&&row<BOARD_SIZE&&col>=0&&col<BOARD_SIZE&&game.cells[row][col]==game.current_player){
            counter++;
            row+=cx;
            col+=cy;
        }
        row=ro;col=co;
        row-=cx;col-=cy; 
        while(row>=0&&row<BOARD_SIZE&&col>=0&&col<BOARD_SIZE&&game.cells[row][col]==game.current_player){
            counter++;
            row-=cx;
            col-=cy;
        }
        if (counter>=WIN_LENGTH)
        {
            game.status=(game.current_player==game.player[0])?GAME_X_WINS:GAME_O_WINS;
            return 1;
        }
        counter=1;
    }       
        return 0;     
}
int check_draw(void){
    if(game.move_count==BOARD_SIZE*BOARD_SIZE){
        game.status=GAME_DRAW;
        return 1;
    }
    return 0;
}
void init_game(void) {
    // 1) 清空棋盘
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            game.cells[r][c] = EMPTY_CELL;   // 用宏，不硬编码字符
        }
    }

    // 2) 两个玩家
    game.player[0] = 'x';
    game.player[1] = 'o';

    // 3) 当前轮到谁：用 player[0] 而不是写死 'x'，改玩家符号时只改一处
    game.current_player = game.player[0];

    // 4) 步数和状态
    game.move_count = 0;
    game.status = GAME_RUNNING;   // 主循环靠这个值判断要不要继续
}

MoveResult place(int row, int col) {
    // --- 防御：游戏已经结束就不再接受落子 ---
    if (game.status != GAME_RUNNING) {
        return MOVE_GAME_OVER;
    }

    // --- 边界检查：必须先判边界，再访问 cells[row][col] ---
    // 反过来的话 cells[-1][5] 就是数组越界（未定义行为，可能直接崩）
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return MOVE_OUT_OF_BOUNDS;
    }

    // --- 占用检查：已经有棋子的格子不能重复落子 ---
    if (game.cells[row][col] != EMPTY_CELL) {
        return MOVE_OCCUPIED;
    }

    // --- 真正落子 ---
    char who = game.current_player;   // 记下这一手是谁下的，后面判胜负要用
    game.cells[row][col] = who;
    game.move_count++;
    
    if(check_win(row,col)){
        return MOVE_ACCEPT;
    }
    if(check_draw()){
        return MOVE_ACCEPT;
    }

    game.current_player = (who == game.player[0]) ? game.player[1] : game.player[0];

    return MOVE_ACCEPT;
}

// main.c —— 初始化、主游戏循环，协调 game 和 console 两个模块
#include "game.h"
#include "console.h"
#include <stdio.h>

/* 游戏结束后在棋盘下面打印最终结果。
   注意：在 step3 的胜负/平局判定补上之前，game.status 永远停在
   GAME_RUNNING，主循环不会退出，所以这个函数暂时跑不到。 */
static void show_result(void) {
    switch (game.status) {
    case GAME_X_WINS: draw_status("X 连成五子，X 获胜！"); break;
    case GAME_O_WINS: draw_status("O 连成五子，O 获胜！"); break;
    case GAME_DRAW:   draw_status("棋盘已下满，平局！");   break;
    default:          draw_status("游戏结束");             break;
    }

    /* 题目要求：游戏结束后打印获胜一方和有效步数 */
    gotoxy(0, BOARD_SIZE * 2 + 3);
    if (game.status == GAME_DRAW)
        printf("有效步数：%d\n", game.move_count);
    else
        printf("获胜方：%c        有效步数：%d\n", game.current_player, game.move_count);

    gotoxy(0, BOARD_SIZE * 2 + 5);
    printf("按任意键退出...\n");
}

int main(void) {
    init_console();   // 保存并设置控制台状态（开鼠标、隐藏光标、切 UTF-8）
    init_game();      // 初始化这一局

    draw_board();
    draw_status("x 先手，点击棋盘落子");

    /* ---------------- 主游戏循环 ---------------- */
    int row, col;
    while (game.status == GAME_RUNNING) {

        /* 等一次有效点击；点在棋盘外就直接提示，不落子 */
        if (!wait_board_click(&row, &col)) {
            draw_status("点在棋盘外了，请点格子内部");
            continue;
        }

        switch (place(row, col)) {
        case MOVE_ACCEPT:
            draw_cell(row, col);   // 只重画这一格，不用整盘重画（不闪、也不破坏状态行）
            if (game.status == GAME_RUNNING) {
                char buf[32];
                snprintf(buf, sizeof(buf), "轮到 %c 落子", game.current_player);//神秘函数，输出到字符串buf中。
                draw_status(buf);
            }
            break;

        case MOVE_OCCUPIED:
            draw_status("这个格子已经有子了");
            break;

        case MOVE_OUT_OF_BOUNDS:
            draw_status("超出棋盘范围");
            break;

        case MOVE_GAME_OVER:
            /* 正常不会走到这里：循环条件已经挡住了。 */
            break;
        }
    }

    /* ---------------- 收尾 ---------------- */
    draw_board();        // 画最终棋盘
    show_result();       // 打印胜负结果和有效步数
    wait_key();          // 等用户按键
    restore_console();   // 还原控制台状态

    return 0;
}

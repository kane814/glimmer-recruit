// console.h —— 控制台相关的结构体和接口
#pragma once

#include <windows.h>   // HANDLE / DWORD / CONSOLE_CURSOR_INFO / INPUT_RECORD ...

/* 进入游戏前的控制台状态。
   退出时必须原样还原，所以这些值要先存下来，不能写死。 */
typedef struct {
    HANDLE h_in;                       // 标准输入句柄
    HANDLE h_out;                      // 标准输出句柄
    DWORD  input_mode;                 // 原来的输入模式
    CONSOLE_CURSOR_INFO cursor_info;   // 原来的光标信息
} GameConsole;

extern GameConsole console;

void init_console(void);      // 保存状态 -> 开启鼠标 -> 隐藏光标
void restore_console(void);   // 把上面保存的状态还原回去

void gotoxy(int x, int y);            // 光标移到屏幕坐标 (x, y)
void draw_board(void);                // 画整个棋盘
void draw_cell(int row, int col);     // 只重画 (row, col) 这一格的棋子
void draw_status(const char *text);   // 在棋盘下方显示一行状态

/* 阻塞等一次有效的棋盘点击。
   成功返回 1 并把棋盘坐标写进 *row / *col；点在棋盘外返回 0。 */
int wait_board_click(int *row, int *col);
void wait_key(void);                  // 等用户按任意键
void set_color(WORD color);
void reset_color(void);
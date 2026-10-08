// console.c —— 控制台交互：初始化/还原、绘制棋盘、读鼠标并换算成棋盘坐标
#include "console.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

GameConsole console;

/* 棋盘占屏幕第 0 ~ BOARD_SIZE*2 行，所以状态行画在它下面一行 */
#define STATUS_LINE  (BOARD_SIZE * 2 + 1)
/* 状态行先擦这么多个空格再写，避免上一次的内容残留（比算 printf 宽度可靠） */
#define STATUS_WIDTH 48

/* 从零开始计数
   第 r 行的棋子在屏幕第 2r + 1 行
   第 c 列的棋子在屏幕第 4c + 2 列
   下面 draw_cell 和 screen_to_board 都依赖这两条，改动请一起改。 */
#define CELL_X(col) (4 * (col) + 2)
#define CELL_Y(row) (2 * (row) + 1)

void init_console(void) {
    console.h_in  = GetStdHandle(STD_INPUT_HANDLE);
    console.h_out = GetStdHandle(STD_OUTPUT_HANDLE);

    /* 1) 切到 UTF-8 代码页。
          源文件是 UTF-8，gcc 生成的字符串字面量也是 UTF-8 字节，
          而 Windows 中文版控制台默认是 936(GBK)，不切的话中文全是乱码。 */
    system("chcp 65001 > nul");

    /* 2) 定窗口大小：棋盘 37 字符宽、19 行高，50x25 够用且留白舒服 */
    system("mode con cols=50 lines=25 > nul");

    /* 3) 读-改-写输入模式。
          不能直接 SetConsoleMode(h, ENABLE_MOUSE_INPUT)，
          那样会把其它位（ENABLE_PROCESSED_INPUT 等）一起清掉。 */
    GetConsoleMode(console.h_in, &console.input_mode);   // 先把原值存起来，console.input_mode是之前结构体的内容物
    DWORD mode = console.input_mode;                     // 在副本上改
    mode |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;  // 让鼠标事件能进输入缓冲区
    mode &= ~ENABLE_QUICK_EDIT_MODE;                     // 关快速编辑，否则点击会被拿去选文本
    SetConsoleMode(console.h_in, mode);                  //两个参数前面的是钥匙，后面修改

    /* 4) 保存原光标信息，只改可见性 */
    GetConsoleCursorInfo(console.h_out, &console.cursor_info);
    CONSOLE_CURSOR_INFO hidden = console.cursor_info;
    hidden.bVisible = FALSE;
    SetConsoleCursorInfo(console.h_out, &hidden);
}

void restore_console(void) {
    SetConsoleMode(console.h_in, console.input_mode);            // 输入模式还原（快速编辑回来了），之前改的是mode，这个本质上是个数，用位做开关
    SetConsoleCursorInfo(console.h_out, &console.cursor_info);   // 光标可见性/大小还原
    gotoxy(0, BOARD_SIZE * 2 + 6);                               // 光标挪到最下面，别停在棋盘中间
}
//光标定位
void gotoxy(int x, int y) {
    COORD p;
    p.X = (SHORT)x;
    p.Y = (SHORT)y;
    SetConsoleCursorPosition(console.h_out, p);
}

void draw_board(void) {
    gotoxy(0, 0);   // 必须的：printf 是从"光标当前位置"开始画的，不归零就会整体错位

    for (int i = 0; i < BOARD_SIZE * 2 + 1; i++) {
        if (i % 2 == 0) {
            /* 偶数行：横线。每格 4 个字符 "+---"，最后补一个 '+'，整行 37 字符 */
            for (int j = 0; j < BOARD_SIZE; j++)
                printf("+---");
            printf("+\n");
        } else {
            /* 奇数行：内容行。屏幕行 i 对应的棋盘行是 i / 2（因为 i = 2r + 1） */
            int r = i / 2;
            for (int j = 0; j < BOARD_SIZE; j++)
                printf("| %c ", game.cells[r][j]);   // 每格也是 4 字符："| 棋子 "
            printf("|\n");
        }
    }
}

void draw_cell(int row, int col) {
    /* 自己负责定位：这个函数的职责就是"把 (row,col) 的棋子画到棋盘上"，
       不该要求调用者先把光标摆好（之前的 bug 就出在这里）。 */
    gotoxy(CELL_X(col), CELL_Y(row));
    printf("%c", game.cells[row][col]);
}

void draw_status(const char *text) {
    gotoxy(0, STATUS_LINE);                  //打印状态，在棋盘下面。
    for (int i = 0; i < STATUS_WIDTH; i++)   // 先把整行擦干净
        putchar(' ');
    gotoxy(0, STATUS_LINE);
    printf("%s", text);
}

/* 字符坐标 -> 棋盘坐标。
   归属规则：第 (r,c) 格认领屏幕区域 X∈[4c, 4c+3]、Y∈[2r, 2r+1]，
   也就是"格子的上边线和左边线算自己"。这样横向纵向规则一致，
   而且点击棋子本身（第 2r+1 行、第 4c+2 列）一定落在正确格子里。
   点在棋盘外返回 0。 */
   //sx和sy都是暂时的变量，没意义。
   //逻辑：先等（wait_board_click），通过screen_to_board判断是否在棋盘上。
static int screen_to_board(int sx, int sy, int *row, int *col) {
    *col = sx / 4;
    *row = sy / 2;

    if (*row < 0 || *row >= BOARD_SIZE || *col < 0 || *col >= BOARD_SIZE)
        return 0;   // 点到棋盘外了（含棋盘最下面那条边线 Y=18 → row=9）
    return 1;
}

int wait_board_click(int *row, int *col) {
    INPUT_RECORD rec;
    DWORD n;

    while (1) {
        ReadConsoleInput(console.h_in, &rec, 1, &n);   // 阻塞：没事件就停在这儿等

        if (rec.EventType != MOUSE_EVENT) continue;     // 键盘、窗口大小变化等一律不关心

        MOUSE_EVENT_RECORD m = rec.Event.MouseEvent;

        /* dwEventFlags != 0 表示移动 / 双击 / 滚轮，不是普通的按下或松开。
           不滤掉的话，鼠标一滑就会触发几十次落子。 */
        if (m.dwEventFlags != 0) continue;

        /* 只认左键"按下"：松开时这一位是 0，会被这里滤掉，
           所以一次完整点击只会落一子，不会重复。 */
        if (!(m.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)) continue;

        return screen_to_board(m.dwMousePosition.X, m.dwMousePosition.Y, row, col);//row和col是两个指针，根据前面两个鼠标事件给其赋值
    }
}

void wait_key(void) {
    INPUT_RECORD rec;
    DWORD n;

    while (1) {
        ReadConsoleInput(console.h_in, &rec, 1, &n);
        /* 只等"按下"的键盘事件（松开也会发一个事件，bKeyDown 为 FALSE） */
        if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown)
            return;
    }
}

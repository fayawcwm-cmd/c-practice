#include <graphics.h>
#include <stdio.h>
#include <time.h>

#define CELL 20          // 每格边长（像素）
#define COLS 32          // 列数 640/20
#define ROWS 22          // 行数 (480-40)/20，顶部留 40 像素显示分数
#define TOP  40          // 顶部信息栏高度
#define MAXLEN 500       // 蛇最长长度

int  sx[MAXLEN], sy[MAXLEN];  // 蛇身各占的格子
int  len;                     // 当前长度
int  dx, dy;                  // 当前移动方向
int  fx, fy;                  // 食物位置
int  score;                   // 分数
bool gameOver;

void reset()   // 初始化/重开一局
{
    len = 3;
    sx[0] = 16; sy[0] = 11;     // 头
    sx[1] = 15; sy[1] = 11;
    sx[2] = 14; sy[2] = 11;
    dx = 1; dy = 0;             // 向右出发
    score = 0;
    gameOver = false;
}

void spawnFood()   // 随机放一个不在蛇身上的食物
{
    bool ok;
    do {
        ok = true;
        fx = rand() % COLS;
        fy = rand() % ROWS;
        for (int i = 0; i < len; i++)
            if (sx[i] == fx && sy[i] == fy) { ok = false; break; }
    } while (!ok);
}

void draw()
{
    setbkcolor(EGERGB(30, 30, 30));
    cleardevice();

    // 顶部分数栏
    char buf[64];
    sprintf(buf, "Score: %d", score);
    setcolor(WHITE);
    outtextxy(10, 12, buf);
    outtextxy(450, 12, "Arrows / WASD");

    // 食物（红色方块）
    setfillcolor(RED);
    bar(fx * CELL + 2, fy * CELL + TOP + 2, (fx + 1) * CELL - 2, (fy + 1) * CELL + TOP - 2);

    // 蛇（头亮绿，身体暗绿）
    for (int i = len - 1; i >= 0; i--)
    {
        if (i == 0) setfillcolor(EGERGB(90, 230, 110));
        else        setfillcolor(EGERGB(40, 160, 70));
        bar(sx[i] * CELL + 1, sy[i] * CELL + TOP + 1,
            (sx[i] + 1) * CELL - 1, (sy[i] + 1) * CELL + TOP - 1);
    }
}

void update()
{
    int nx = sx[0] + dx;
    int ny = sy[0] + dy;

    // 撞墙或咬到自己（尾巴那一格马上要让出来，所以不检查最后一节）
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) { gameOver = true; return; }
    for (int i = 0; i < len - 1; i++)
        if (sx[i] == nx && sy[i] == ny) { gameOver = true; return; }

    if (nx == fx && ny == fy)   // 吃到食物：整体变长，重新放食物
    {
        for (int i = len; i > 0; i--) { sx[i] = sx[i - 1]; sy[i] = sy[i - 1]; }
        sx[0] = nx; sy[0] = ny;
        len++;
        score += 10;
        spawnFood();
    }
    else                        // 普通移动：身体跟上，尾巴消失
    {
        for (int i = len - 1; i > 0; i--) { sx[i] = sx[i - 1]; sy[i] = sy[i - 1]; }
        sx[0] = nx; sy[0] = ny;
    }
}

int main()
{
    initgraph(640, 480);
    setfontbkcolor(BLACK);      // 文字背景不透明（黑），避免和花屏重叠
    srand((unsigned)time(NULL));

    while (true)                // 外层：一局结束可按 R 再来
    {
        reset();
        spawnFood();

        while (!gameOver)       // 内层：游戏主循环
        {
            if (kbhit())
            {
                int k = getch();
                if (k == key_esc) { closegraph(); return 0; }
                // 禁止 180° 掉头
                if      ((k == key_up    || k == 'w') && dy != 1)  { dx = 0;  dy = -1; }
                else if ((k == key_down  || k == 's') && dy != -1) { dx = 0;  dy = 1;  }
                else if ((k == key_left  || k == 'a') && dx != 1)  { dx = -1; dy = 0;  }
                else if ((k == key_right || k == 'd') && dx != -1) { dx = 1;  dy = 0;  }
            }

            update();
            draw();

            int fps = 10 + score / 30;      // 每吃 3 个食物提速一档
            if (fps > 25) fps = 25;
            delay_fps(fps);
        }

        // 结束画面
        setcolor(RED);
        outtextxy(250, 170, "GAME OVER");
        char buf[64];
        sprintf(buf, "Score: %d", score);
        setcolor(WHITE);
        outtextxy(270, 210, buf);
        outtextxy(190, 250, "Press R to restart, ESC to quit");

        while (true)            // 等 R 或 ESC
        {
            if (!kbhit()) { delay_ms(10); continue; }
            int k = getch();
            if (k == 'r' || k == 'R') break;
            if (k == key_esc) { closegraph(); return 0; }
        }
    }
}
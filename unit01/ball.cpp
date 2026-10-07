#include <graphics.h>
#include <stdio.h>     // sprintf 需要
#include <time.h>

int main()
{
    // 初始化窗口
    initgraph(640, 480);
    setbkcolor(BLACK);
    cleardevice();

    // 球的初始状态
    int x = 100, y = 100;      // 球心坐标
    int vx = 5, vy = 4;        // 速度（x、y 方向）
    int r = 20;                // 半径
    int count = 0;             // 碰撞计数

    // 所有可用的颜色，撞墙就随机换一个
    color_t colors[] = {RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE};
    srand((unsigned)time(NULL));

    setcolor(WHITE);
    setfontbkcolor(BLACK);     // ← 修正：EGE 里是 setfontbkcolor，不是 settextbkcolor

    // 主循环：窗口开着就一直跑
    while (is_run())
    {
        // 键盘交互：ESC(27) 退出，空格暂停/继续
        if (kbhit())
        {
            int key = getch();
            if (key == 27) break;
            if (key == ' ')
            {
                outtextxy(220, 220, "PAUSED - press any key");
                getch();
            }
        }

        // 更新位置
        x += vx;
        y += vy;

        // 撞墙检测：反弹 + 换色 + 计数
        if (x - r < 0 || x + r > 640) { vx = -vx; count++; setfillcolor(colors[rand() % 7]); }
        if (y - r < 0 || y + r > 480) { vy = -vy; count++; setfillcolor(colors[rand() % 7]); }

        // 重绘画面
        cleardevice();
        fillellipse(x, y, r, r);                    // 画球

        // 显示信息
        char info[64];
        sprintf(info, "Bounces: %d", count);
        outtextxy(10, 10, info);
        outtextxy(10, 30, "SPACE: pause   ESC: quit");

        // 控制帧率，约 60 FPS
        delay_ms(16);
    }

    closegraph();
    return 0;
}
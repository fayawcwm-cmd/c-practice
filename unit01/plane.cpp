#include <graphics.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>      // GetAsyncKeyState（检测"按住"的键）

#define W 640
#define H 480
#define MAXB 40           // 子弹上限
#define MAXE 25           // 敌机上限
#define MAXS 70           // 星星数量

int  bx[MAXB], by[MAXB];  bool bact[MAXB];   // 子弹
int  ex[MAXE], ey[MAXE], ev[MAXE]; bool eact[MAXE]; // 敌机（x,y,速度）
int  sx[MAXS], sy[MAXS], sv[MAXS];            // 背景星星
int  px, py;                                  // 玩家飞机位置
int  hp, score, inv;                          // 生命 / 分数 / 无敌剩余帧
int  fireCd, spawnCd;                         // 开火冷却 / 出怪间隔

void init()
{
    px = W / 2; py = H - 60;
    hp = 3; score = 0; inv = 0;
    fireCd = 0; spawnCd = 0;
    for (int i = 0; i < MAXB; i++) bact[i] = false;
    for (int i = 0; i < MAXE; i++) eact[i] = false;
    for (int i = 0; i < MAXS; i++)
    { sx[i] = rand() % W; sy[i] = rand() % H; sv[i] = 1 + rand() % 3; }
}

void fire()
{
    if (fireCd > 0) return;
    for (int i = 0; i < MAXB; i++)
        if (!bact[i])
        {
            bact[i] = true; bx[i] = px; by[i] = py - 22;
            fireCd = 8;                       // 8 帧一发 = 连发
            return;
        }
}

void spawnEnemy()
{
    for (int i = 0; i < MAXE; i++)
        if (!eact[i])
        {
            eact[i] = true;
            ex[i] = 20 + rand() % (W - 40);
            ey[i] = -20;
            ev[i] = 2 + rand() % 3 + score / 800;   // 分数越高飞得越快
            return;
        }
}

void update()
{
    // —— 玩家控制：GetAsyncKeyState 能检测"按住不放"，比 getch 顺滑 ——
    int sp = 6;
    if (GetAsyncKeyState(VK_LEFT))  px -= sp;
    if (GetAsyncKeyState(VK_RIGHT)) px += sp;
    if (GetAsyncKeyState(VK_UP))    py -= sp;
    if (GetAsyncKeyState(VK_DOWN))  py += sp;
    if (px < 15) px = 15; if (px > W - 15) px = W - 15;
    if (py < 40) py = 40; if (py > H - 20) py = H - 20;

    if (GetAsyncKeyState(VK_SPACE)) fire();
    if (fireCd > 0) fireCd--;

    // —— 出怪：间隔随分数缩短（40 帧 → 最快 10 帧）——
    if (--spawnCd <= 0)
    {
        spawnEnemy();
        int t = score / 400; if (t > 30) t = 30;
        spawnCd = 40 - t;
    }

    // —— 星星滚动 ——
    for (int i = 0; i < MAXS; i++)
    {
        sy[i] += sv[i];
        if (sy[i] > H) { sy[i] = 0; sx[i] = rand() % W; }
    }

    // —— 子弹上移 ——
    for (int i = 0; i < MAXB; i++)
        if (bact[i]) { by[i] -= 10; if (by[i] < 0) bact[i] = false; }

    // —— 敌机下移 ——
    for (int i = 0; i < MAXE; i++)
        if (eact[i]) { ey[i] += ev[i]; if (ey[i] > H + 20) eact[i] = false; }

    // —— 子弹打敌机：圆形碰撞（距离平方 < 半径和平方）——
    for (int i = 0; i < MAXE; i++) if (eact[i])
        for (int j = 0; j < MAXB; j++) if (bact[j])
        {
            int ddx = bx[j] - ex[i], ddy = by[j] - ey[i];
            if (ddx * ddx + ddy * ddy < 24 * 24)
            { eact[i] = false; bact[j] = false; score += 10; break; }
        }

    // —— 敌机撞玩家 ——
    if (inv > 0) inv--;
    else
        for (int i = 0; i < MAXE; i++) if (eact[i])
        {
            int ddx = ex[i] - px, ddy = ey[i] - py;
            if (ddx * ddx + ddy * ddy < 26 * 26)
            { eact[i] = false; hp--; inv = 90; break; }  // 90 帧无敌
        }
}

void draw()
{
    setbkcolor(EGERGB(10, 10, 25));
    cleardevice();

    // 星空背景
    for (int i = 0; i < MAXS; i++)
        putpixel(sx[i], sy[i], EGERGB(120, 120, 150));

    // 敌机：红圆 + 白"眼睛"
    for (int i = 0; i < MAXE; i++) if (eact[i])
    {
        setfillcolor(RED);
        fillellipse(ex[i], ey[i], 14, 14);
        setfillcolor(WHITE);
        fillellipse(ex[i], ey[i] + 4, 5, 5);
    }

    // 子弹
    setfillcolor(YELLOW);
    for (int i = 0; i < MAXB; i++) if (bact[i])
        bar(bx[i] - 2, by[i] - 8, bx[i] + 2, by[i] + 8);

    // 玩家飞机：无敌时闪烁（inv/6 % 2 交替隐藏）
    if (inv <= 0 || (inv / 6) % 2 == 0)
    {
        setcolor(CYAN);
        line(px - 18, py + 12, px, py - 6);
        line(px + 18, py + 12, px, py - 6);
        setfillcolor(EGERGB(80, 200, 255));
        fillellipse(px, py, 8, 20);
    }

    // HUD：分数 + 血条
    char buf[64];
    sprintf(buf, "Score: %d", score);
    setcolor(WHITE);
    outtextxy(10, 10, buf);
    for (int i = 0; i < hp; i++)
    {
        setfillcolor(RED);
        bar(10 + i * 30, 32, 32 + i * 30, 42);
    }
}

int main()
{
    initgraph(W, H);
    setfontbkcolor(BLACK);
    srand((unsigned)time(NULL));

    while (true)
    {
        init();
        while (hp > 0)
        {
            if (GetAsyncKeyState(VK_ESCAPE)) { closegraph(); return 0; }
            update();
            draw();
            delay_fps(60);
        }

        // 结束画面
        setcolor(RED);
        outtextxy(240, 190, "GAME OVER");
        char buf[64];
        sprintf(buf, "Score: %d", score);
        setcolor(WHITE);
        outtextxy(260, 230, buf);
        outtextxy(180, 270, "Press R to restart, ESC to quit");

        while (true)
        {
            if (!kbhit()) { delay_ms(10); continue; }
            int k = getch();
            if (k == 'r' || k == 'R') break;
            if (k == key_esc) { closegraph(); return 0; }
        }
    }
}
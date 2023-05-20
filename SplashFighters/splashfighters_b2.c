//4J 439 湯浅太貴

//タイマーの読み込み速度によって敵の当たり判定ができない可能性がある
//敵が画面外に行く対処法は，配列を大きくすることである．
//Pを連打しないでください...(タイマーの仕様上，カウントがうまくされないことがある)
//↑
//解決法は，プレイヤーと敵を同じタイミングで動かせるようなゲームにすること
//または，インクを塗るキー入力にタイマーを設けること

#include <gl/glut.h>
#include <gl/glpng.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WWIDTH 320  //ウィンドウサイズ(横)
#define WHEIGHT 352 //ウィンドウサイズ(縦)

#define Pimage 2     //プレイヤーの画像数
#define Eimage 2     //敵の画像数
#define Mapimage 5   //マップの画像数
#define Gameimage 10 //ゲーム画像(タイトル等)

#define HEIGHT 10    //マップの縦の長さ11
#define WIDTH 11     //マップの横の長さ10+NULL
#define MAXMapink 64 //マップ上に塗れる最大のインク数
#define GameTime 30  //塗り合う時間(秒)

#define Piposx 32    //プレイヤーの初期位置x座標
#define Piposy 32    //プレイヤーの初期位置y座標
#define MAXPink 2000 //プレイヤーのインクの数

#define Eiposx 256   //敵の初期位置x座標
#define Eiposy 256   //敵の初期位置y座標
#define Espeed 350   //敵のスピード(350ms)
#define MAXEink 3000 //敵のインクの数

#define STLNUM 255 //文字列の大きさ

//プロトタイプ宣言
void InitV(void);
void Pngread(void);
void PutSprite(int, int, int, pngInfo *);
void StlDisplay(double, double, char *);
void GameStartScreen(void);
void GameContentsScreen(void);
void GameResultScreen(void);
void GameEndScreen(void);
void Display(void);
void Pinkcount(void);
void Einkcount(void);
void Timer1(int);
void Timer2(int);
void Timer3(int);
int movecheck(int, int, int);
void Keyboard(unsigned char, int, int);
void SpecialKey(int, int, int);
void Reshape(int, int);

//PNGイメージを読み込むための変数
//screen(inf)...ゲーム画面のpng用
//me(inf)...プレイヤーのpng用
//fd(inf)...背景のpng用
//en(inf)...敵のpng用
GLuint screen[Gameimage], me[Pimage], fd[Mapimage], en[Eimage];
pngInfo screeninf[Gameimage], meinf[Pimage], fdinf[Mapimage], eninf[Eimage];

//ゲームの画面制御
int GameScreen = 0;

//pngファイル読み込み用および文字表示用
char stl[STLNUM];

//塗り合う時間(秒)
int Stime = GameTime;

//pが押されたか判定するフラグ
int flag_keyp = 0;

//プレイヤーのパラメータ
int px = Piposx, py = Piposy; //プレイヤー表示用の座標
int pt = 0;                   //アニメーションのカウンタ(0か1)
int dir = 2;                  //プレイヤーの向き(上,右,下,左=0,1,2,3)
//プレイヤーのインク描画のための変数
int pinkx[MAXPink], pinky[MAXPink];
//プレイヤーのインクの数
int pinkcount = 0;
//プレイヤーがマップ上にインクを塗った数(0から64)
int pinkcount2 = 0;

//敵のパラメータ
int ex = Eiposx, ey = Eiposy; //敵表示用の座標
int et = 0;                   //アニメーションのカウンタ(0か1)
//敵のインク描画のための変数
int einkx[MAXEink], einky[MAXEink];
//敵のインクの数
int einkcount = 0;
//敵がマップ上にインクを塗った数(0から64)
int einkcount2 = 0;
//敵のスピード
int espeed = Espeed;

//マップデータ1
char map[HEIGHT][WIDTH] = {
    "ABBBBBBBBA",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "ABBBBBBBBA",
};

//マップデータ2(初期化用)
char map2[HEIGHT][WIDTH] = {
    "ABBBBBBBBA",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "BCCCCCCCCB",
    "ABBBBBBBBA",
};

//
//main関数
//
int main(int argc, char **argv)
{
    //  一般的な準備
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH, WHEIGHT);
    glutCreateWindow("Splash Fighters");
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
    glClearColor((0.0f / 255.0f), (230.0f / 255.0f), (154.0f / 255.0f), 0);
    //  テクスチャのアルファチャネルを有効にする設定
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    //png画像の読み込み
    Pngread();

    //  コールバック関数の登録
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKey);
    glutTimerFunc(550, Timer1, 0);
    glutTimerFunc(espeed, Timer2, 0);
    glutTimerFunc(1000, Timer3, 0);

    //  イベントループ突入
    glutMainLoop();

    return (0);
}

//
//変数初期化関数
//Initialize Variable
void InitV(void)
{
    int i, j;

    //ゲームの画面制御
    GameScreen = 0;
    //塗り合う時間(秒)
    Stime = GameTime;
    //pが押されたか判定するフラグ
    flag_keyp = 0;

    //マップデータ
    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            map[i][j] = map2[i][j];
        }
    }

    //プレイヤーのパラメータ
    px = Piposx; //プレイヤー表示用の座標
    py = Piposy;
    pt = 0;  //アニメーションのカウンタ(0か1)
    dir = 2; //プレイヤーの向き(上,右,下,左=0,1,2,3)

    //プレイヤーのインクの数
    pinkcount = 0;
    //プレイヤーがマップ上にインクを塗った数(0から64)
    pinkcount2 = 0;

    //敵のパラメータ
    ex = Eiposx; //敵の表示用の座標
    ey = Eiposy;
    et = 0; //アニメーションのカウンタ(0か1)

    //敵のインクの数
    einkcount = 0;
    //敵がマップ上にインクを塗った数(0から64)
    einkcount2 = 0;
    //敵のスピード
    espeed = Espeed;
}

//
//png画像を読み込むための関数
//
void Pngread()
{
    int i;

    //PNGイメージの読み込み
    for (i = 0; i < Pimage; i++)
    {
        sprintf(stl, ".\\Character\\playre%1d.png", i);
        me[i] = pngBind(stl, PNG_NOMIPMAP, PNG_ALPHA, &meinf[i],
                        GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

    for (i = 0; i < Mapimage; i++)
    {
        sprintf(stl, ".\\Map\\map%1d.png", i);
        fd[i] = pngBind(stl, PNG_NOMIPMAP, PNG_ALPHA, &fdinf[i],
                        GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

    for (i = 0; i < Eimage; i++)
    {
        sprintf(stl, ".\\Character\\enemy%1d.png", i);
        en[i] = pngBind(stl, PNG_NOMIPMAP, PNG_ALPHA, &eninf[i],
                        GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }

    for (i = 0; i < Gameimage; i++)
    {
        sprintf(stl, ".\\Title\\Game%1d.png", i);
        screen[i] = pngBind(stl, PNG_NOMIPMAP, PNG_ALPHA, &screeninf[i],
                            GL_CLAMP, GL_NEAREST, GL_NEAREST);
    }
}

//
//num番のPNG画像を座標(x,y)に表示する
//
void PutSprite(int num, int x, int y, pngInfo *info)
{
    int w, h; //  テクスチャの幅と高さ

    w = info->Width; //  テクスチャの幅と高さを取得する
    h = info->Height;

    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, num);

    glBegin(GL_QUADS); //  幅w, 高さhの四角形

    glTexCoord2i(0, 0); //画像の左上を0,0
    glVertex2i(x, y);   //左上と対応させる

    glTexCoord2i(0, 1);
    glVertex2i(x, y + h);

    glTexCoord2i(1, 1);
    glVertex2i(x + w, y + h);

    glTexCoord2i(1, 0);
    glVertex2i(x + w, y);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//
//文字を表示する関数
//
void StlDisplay(double x, double y, char *s)
{
    int i;
    sprintf(stl, s);

    glRasterPos2d(x, y);

    for (i = 0; i < strlen(stl); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, stl[i]);
    }
}

//
//ゲームのスタート画面
//
void GameStartScreen(void)
{
    //スタート画面表示
    PutSprite(screen[2], 230, 85, &screeninf[2]);
    PutSprite(screen[3], 23, 270, &screeninf[3]);
    PutSprite(screen[7], 2, 20, &screeninf[7]);
    PutSprite(screen[9], 233, 275, &screeninf[9]);
    PutSprite(me[0], 10, 63, &meinf[0]);
    PutSprite(en[0], 280, 63, &eninf[0]);
    PutSprite(screen[0], 0, 80, &screeninf[0]);

    sprintf(stl, "ver.1.0.0");
    StlDisplay(240, 20, stl);
    sprintf(stl, "2021  Miki");
    StlDisplay(5, 341, stl);
    sprintf(stl, "Play: Press 'e'");
    StlDisplay(200, 341, stl);
}

//
//ゲームのコンテンツ画面
//
void GameContentsScreen(void)
{
    int i, j, k;

    if (flag_keyp == 1)
    {
        //プレイヤーのインクの数を更新
        //if (pinkcount <= MAXPink)
        if (pinkcount < MAXPink)
        {
            pinkcount++;
        }
        //プレイヤーのインク描画座標を取得
        //ここで計算する理由は，
        //タイマーに関与しない点
        pinkx[pinkcount] = px / 32;
        pinky[pinkcount] = py / 32;
        //プレイヤーのインクマスの数をカウント
        Pinkcount();

        //プレイヤーのインクをmap[][]の'D'として表示
        map[pinky[pinkcount]][pinkx[pinkcount]] = 'D';
        flag_keyp = 0;
    }

    //敵のインクをmap[][]の'E'として表示
    //表示上の問題(初期値(0,0)に表示される問題)をなくすため,
    //einkcountが0のとき以外敵のインクを表示
    //if (einkcount != 0)
    //{
    //敵のインク描画座標を取得
    einkx[einkcount] = ex / 32;
    einky[einkcount] = ey / 32;
    //敵のインクマスの数をカウント
    Einkcount();

    map[einky[einkcount]][einkx[einkcount]] = 'E';
    //}

    //printf("p:%d\n", pinkcount2);
    printf("e:%d\n", einkcount2);

    //マップを表示
    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            k = map[i][j];
            PutSprite(fd[k - 'A'], j * 32, i * 32, &fdinf[k - 'A']);
        }
    }

    //プレイヤー描画処理
    PutSprite(me[pt], px, py, &meinf[pt]);

    //敵描画処理
    PutSprite(en[et], ex, ey, &eninf[et]);

    //文字の表示
    if (Stime > 0)
    {
        sprintf(stl, "Countdown %.ds", Stime);
        StlDisplay(100, 341, stl);
    }
    else
    {
        sprintf(stl, "Time up!! Press 'e'");
        StlDisplay(90, 341, stl);
    }
}

//
//ゲームのリザルト画面
//
void GameResultScreen(void)
{
    PutSprite(fd[3], 80, 150, &fdinf[3]);
    PutSprite(fd[4], 180, 150, &fdinf[4]);

    //プレイヤーが勝利
    if (pinkcount2 > einkcount2)
    {
        sprintf(stl, "Win!");
        StlDisplay(5, 20, stl);
    }
    //プレイヤーが負け
    else if (einkcount2 > pinkcount2)
    {
        sprintf(stl, "Lose...");
        StlDisplay(5, 20, stl);
    }
    //引き分け
    else
    {
        sprintf(stl, "Draw:)");
        StlDisplay(5, 20, stl);
    }

    //プレイヤーのインクマスがゼロのとき
    if (pinkcount2 == 0)
    {
        //プレイヤーのインクマスの数
        sprintf(stl, "x 0");
        StlDisplay(117, 180, stl);
        //敵のインクマスの数
        sprintf(stl, "x %.d", einkcount2);
        StlDisplay(217, 180, stl);
    }
    else
    {
        //プレイヤーのインクマスの数
        sprintf(stl, "x %.d", pinkcount2);
        StlDisplay(117, 180, stl);
        //敵のインクマスの数
        sprintf(stl, "x %.d", einkcount2);
        StlDisplay(217, 180, stl);
    }

    sprintf(stl, "Next: Press 'e'");
    StlDisplay(200, 341, stl);
}

//
//ゲームのエンド画面
//
void GameEndScreen(void)
{
    //エンド画面表示
    PutSprite(screen[4], 0, 5, &screeninf[4]);
    PutSprite(screen[5], 100, 275, &screeninf[5]);
    PutSprite(screen[6], 9, 275, &screeninf[6]);
    PutSprite(screen[8], 230, 160, &screeninf[8]);
    PutSprite(screen[1], 0, 30, &screeninf[1]);

    sprintf(stl, "Play again: Press 'e'");
    StlDisplay(5, 320, stl);
    sprintf(stl, "End game: Press 'Esc'");
    StlDisplay(5, 341, stl);
}

//
//ウィンドウの表示内容を更新する
//
void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (GameScreen == 0)
    {
        GameStartScreen();
    }
    else if (GameScreen == 1)
    {
        GameContentsScreen();
    }
    else if (GameScreen == 2)
    {
        GameResultScreen();
    }
    else if (GameScreen == 3)
    {
        GameEndScreen();
    }
    glFlush();
}

//
//プレイヤーのインクマスをカウントするための関数
//
void Pinkcount(void)
{
    //mapに何も塗られていないとき
    if (map[pinky[pinkcount]][pinkx[pinkcount]] == 'C')
    {
        //プレイヤーがmap上に塗ったインクの数を更新
        if (pinkcount2 < MAXMapink)
        {
            pinkcount2++;
        }
    }
    //敵のインクが塗られているとき
    else if (map[pinky[pinkcount]][pinkx[pinkcount]] == 'E')
    {
        //プレイヤーがmap上に塗ったインクの数を更新
        if (pinkcount2 < MAXMapink)
        {
            pinkcount2++;
        }
        if (einkcount2 > 0)
        {
            einkcount2--;
        }
    }
    else
    {
        pinkcount2 = pinkcount2;
    }
}

//
//敵のインクマスをカウントするための関数
//
void Einkcount(void)
{
    //mapに何も塗られていないとき
    if (map[einky[einkcount]][einkx[einkcount]] == 'C')
    {
        //敵がmap上に塗ったインクの数を更新
        if (einkcount2 <= MAXMapink)
        {
            einkcount2++;
        }
    }
    //プレイヤーのインクが塗られているとき
    else if (map[einky[einkcount]][einkx[einkcount]] == 'D')
    {
        //敵がmap上に塗ったインクの数を更新
        if (einkcount2 <= MAXMapink)
        {
            einkcount2++;
        }
        if (pinkcount2 > 0)
        {
            pinkcount2--;
        }
    }
    else
    {
        einkcount2 = einkcount2;
    }
}

//
//プレイヤーのアニメーションのためのタイマー割込みルーチン
//
void Timer1(int t)
{
    if ((GameScreen == 1) && (Stime != 0))
    {
        //プレイヤーのアニメーション
        pt = 1 - pt;
    }
    glutPostRedisplay();
    glutTimerFunc(550, Timer1, 0);
}

//
//enemy(敵)を動かすためのタイマー割り込みルーチン
//
void Timer2(int t)
{
    int r;
    int j;
    if ((GameScreen == 1) && (Stime != 0))
    {
        r = random() % 8;
        if ((0 <= r) && (r <= 3))
        {
            j = movecheck(ex, ey, r);
            if ((j == 1) && (r == 0))
                ey -= 32;
            if ((j == 1) && (r == 1))
                ex += 32;
            if ((j == 1) && (r == 2))
                ey += 32;
            if ((j == 1) && (r == 3))
                ex -= 32;
        }

        if (einkcount <= MAXEink)
        {
            einkcount++;
        }
        /*//敵のインク描画座標を取得
        einkx[einkcount] = ex / 32;
        einky[einkcount] = ey / 32;
        //敵のインクマスの数をカウント
        Einkcount();*/

        //敵のアニメーション
        et = 1 - et;
    }

    glutPostRedisplay();

    //衝突判定
    if ((abs(px - ex) < 32) && (abs(py - ey) < 32))
    {
        if (espeed > 110)
        {
            //プレイヤーが敵と衝突すると
            //1回につき敵が60ms速くなる
            //最大速度は110ms(4回衝突時)
            espeed -= 60;
        }
        fflush(NULL);
    }
    glutTimerFunc(espeed, Timer2, 0);
}

//
//インクを塗り合う時間のためのタイマー割り込みルーチン
//
void Timer3(int t)
{
    if (GameScreen == 1)
    {
        //1秒刻みでカウント
        if (Stime > 0)
        {
            Stime--;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(1000, Timer3, 0);
}

//
//現在の座標(x,y)から方向dに移動可能なら1を返す
//
int movecheck(int x, int y, int d)
{
    int fg = 0;   //フラグ
    int cx1, cy1; //チェックするmap[][]その1
    int cx2, cy2; //チェックするmap[][]その2

    //上を向いていて,かつ上に進める座標があるとき
    if ((d == 0) && (y > 0))
    {
        fg = 1;
        cx1 = x / 32;
        cy1 = (y - 1) / 32;
        cx2 = cx1;
        cy2 = cy1;
        if (x % 32 != 0)
            cx2++;
    }

    //下を向いていて,かつ下に進める座標があるとき
    //下には32×32の壁があるためy座標は320-32の値
    if ((d == 2) && (y < 288))
    {
        fg = 1;
        cx1 = x / 32;
        cx2 = cx1;
        cy1 = y / 32 + 1;
        cy2 = cy1;
        if (x % 32 != 0)
            cx2++;
    }

    //左を向いていて，かつ左に進める座標があるとき
    if ((d == 3) && (x > 0))
    {
        fg = 1;
        cx1 = (x - 1) / 32;
        cy1 = y / 32;
        cx2 = cx1;
        cy2 = cy1;
        if (y % 32 != 0)
            cy2++;
    }
    //右を向いていて,かつ右に進める座標があるとき
    if ((d == 1) && (x < 288))
    {
        fg = 1;
        cx1 = x / 32 + 1;
        cy1 = y / 32;
        cx2 = cx1;
        cy2 = cy1;
        if (y % 32 != 0)
            cy2++;
    }

    if (fg == 1)
    {
        if (((map[cy1][cx1] == 'A') || (map[cy2][cx2] == 'A')) || ((map[cy1][cx1] == 'B') || (map[cy2][cx2] == 'B')))
            fg = 0;
    }

    return (fg);
}

//
//キー入力の処理
//' '内はキーで指定
void Keyboard(unsigned char key, int x, int y)
{
    if (key == 'e')
    {
        //ゲーム中は次の画面に進めないように設定
        if ((GameScreen == 1) && (Stime != 0))
        {
            GameScreen = 1;
        }
        else if (GameScreen == 3)
        {
            //エンド画面でeを押されたら
            //ゲームスタート画面に戻る
            //初期化される
            InitV();
        }
        else
        {
            GameScreen++;
        }
    }

    if (GameScreen == 1)
    {
        //プレイヤーと敵が重なっているときは
        //プレイヤーはインクを塗れない
        //重なったときのインクカウント時のエラー対処
        if ((px * py) != (ex * ey))
        {
            //pキー
            if (key == 'p')
            {
                flag_keyp = 1;

                //プレイヤーのインクの数を更新
                //if (pinkcount <= MAXPink)
                /*if (pinkcount < MAXPink)
                {
                    pinkcount++;
                }
                //プレイヤーのインク描画座標を取得
                //ここで計算する理由は，
                //タイマーに関与しない点
                pinkx[pinkcount] = px / 32;
                pinky[pinkcount] = py / 32;
                //プレイヤーのインクマスの数をカウント
                Pinkcount();*/
            }
        }
    }

    //ESCキー
    if (key == 27)
    {
        printf("Escキーで終了しました.\n");
        exit(0);
    }
}

//
//プレイヤーの移動キー設定関数
//(上,左,下,右)は(w,a,s,d)に対応
void SpecialKey(int key, int x, int y)
{
    int dx = 0, dy = 0; //座標の増分

    if ((GameScreen == 1) && (Stime != 0))
    {
        switch (key)
        {
        case GLUT_KEY_UP:
            dir = 0;
            if (movecheck(px, py, dir) == 1)
                dy = -1;
            break;
        case GLUT_KEY_DOWN:
            dir = 2;
            if (movecheck(px, py, dir) == 1)
                dy = 1;
            break;
        case GLUT_KEY_LEFT:
            dir = 3;
            if (movecheck(px, py, dir) == 1)
                dx = -1;
            break;
        case GLUT_KEY_RIGHT:
            dir = 1;
            if (movecheck(px, py, dir) == 1)
                dx = 1;
            break;
        default:
            dx = 0;
            dy = 0;
        }
        px += dx * 32;
        py += dy * 32;
    }
    glutPostRedisplay();
}

//
//  ウィンドウのサイズ変更が発生したときに座標系を再設定する関数
//
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glScaled(1, -1, 1);
    glTranslated(0, -h, 0);
    glutReshapeWindow(WWIDTH, WHEIGHT);
}
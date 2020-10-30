// ConsoleApplication5.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <vector>
#include <cstdlib>
using namespace std;

#define BOARDSIZE		16
#define EMPTY			0
#define BLACK_STONE		1		
#define WHITE_STONE		2
#define WALL			3
#define SUGGEST			4
#define SUGGESTJ		5
#define CANMV			10

int match = 1;			//対戦回数
int logs = 0;				//ログの設定（0:すべて表示,1:最後の盤面と結果のみ表示,2:結果のみ表示）


int halmaBoard[BOARDSIZE + 1][BOARDSIZE + 1];
int canBoard[BOARDSIZE + 1][BOARDSIZE + 1];
int preBoard[BOARDSIZE + 1][BOARDSIZE + 1];
int mvBoard[BOARDSIZE + 1][BOARDSIZE + 1];
int jumpBoard[BOARDSIZE + 1][BOARDSIZE + 1];
int player = BLACK_STONE;
int selectX, selectY, selectnX, selectnY, selectpX, selectpY, selectcX, selectcY, cb, jb, jp, whs, bls, empw, empb, gameend, mode, jumpcount, exitgame, cmnum, test;
int maxsubnumx;
int maxsubnumy;
int nt;
int fn;
int nearx, neary;
int slow,far,fary;
int blackwin, whitewin;
int whsn, blsn;
unsigned int presize;
char jump[1];
int turnnum,turnnumt,turnmaxw,turnminw,turnavew, turnmaxb, turnminb, turnaveb;

std::vector<int> cpx;
std::vector<int> cpy;
std::vector<int> cpj0x;
std::vector<int> cpj0y;
std::vector<int> empx;
std::vector<int> empy;
std::vector<int> mvnx;
std::vector<int> mvny;
std::vector<int> farp;
std::vector<int> sjump;

void Menu();
void SetBoard(int i);
void ShowBoard();
void Bflow();
void MoveCheck();
void ShowCBoard();
void BoardCheck();
void PreCheck();
void Select(int i);
void MovePiece();
void ChangePlayer();
void WinCheck();
void SwPlayer(int i);
void GameClear();
void AI_A(int i);
void AI_B(int i);
void AI_C(int i);
void AI_D(int i);

//ボードの点数
int aiBoard[BOARDSIZE + 1][BOARDSIZE + 1] = {
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
	{ 0,39,42,46,49,53,56,60,63,67,70,71,85,89,92,96,99 },
	{ 0,36,40,43,47,50,54,57,61,64,68,71,83,86,90,93,96 },
	{ 0,34,37,41,44,48,51,55,58,62,65,69,71,84,87,90,92 },
	{ 0,31,35,38,42,45,49,52,56,59,63,66,70,71,84,86,89 },
	{ 0,29,32,36,39,43,46,50,53,57,60,64,67,70,71,83,85 },
	{ 0,26,30,33,37,40,44,47,51,54,58,61,64,66,69,71,71 },
	{ 0,24,27,31,34,38,41,45,48,52,55,58,60,63,65,68,70 },
	{ 0,21,25,28,32,35,39,42,46,49,52,54,57,59,62,64,67 },
	{ 0,19,22,26,29,33,36,40,43,46,48,51,53,56,58,61,63 },
	{ 0,16,20,23,27,30,34,37,40,42,45,47,50,52,55,57,60 },
	{ 0,14,17,21,24,28,31,34,36,39,41,44,46,49,51,54,56 },
	{ 0,11,15,18,22,25,28,30,33,35,38,40,43,45,48,50,53 },
	{ 0, 9,12,16,19,22,24,27,29,32,34,37,39,42,44,47,49 },
	{ 0, 6,10,13,16,18,21,23,26,28,31,33,36,38,41,43,46 },
	{ 0, 4, 7,10,12,15,17,20,22,25,27,30,32,35,37,40,42 },
	{ 0, 1, 4, 6, 9,11,14,16,19,21,24,26,29,31,34,36,39 },

};


inline void InitRand()
{
	struct timeb timebuf;
	ftime(&timebuf);
	srand((unsigned int)timebuf.millitm);
	rand(); rand(); rand(); rand(); rand();
}

int main()
{
	int nu = 0;
	Menu();

	turnminb = 10000;
	turnminw = 10000;

				//pvpはmode１,pvaiはmode2と3,aivaiはmode4,
	while (nu < match) {
		SetBoard(0);//ボードを初期化
		gameend = 0;

		do {
			turnnum++;
			turnnumt = (turnnum + 1) / 2;
			if (logs == 0) {
				printf("turn:%d\n", turnnumt);
			}
			Bflow();
			

		} while (gameend == 0);
		nu++;
		turnnum = 0;
	}
	if (blackwin!=0) {
		turnaveb /= blackwin;
	}
	if (whitewin!=0) {
		turnavew /= whitewin;
	}
	
	//printf("b%d / w%d\n", blackwin, whitewin);
	printf("black%d  max%d  min%d  ave%d\n", blackwin, turnmaxb,turnminb,turnaveb);
	printf("white%d  max%d  min%d  ave%d\n", whitewin, turnmaxw, turnminw, turnavew);
	scanf_s("%d", &exitgame);

}

//対戦のフロー
void Bflow() {
	SetBoard(2);	//直前のボードの状態を保存
	do
	{

		SetBoard(1);	//移動できるマスの情報をリセット
		SetBoard(3);

		if (jumpcount == 0) {	//ジャンプ回数が0の時（ジャンプ移動していないとき）
			PreCheck();			//すべての駒をチェックして移動できる駒の情報を保存
			if (logs == 0) {
				ShowBoard();		//ボードを表示
			}
			SwPlayer(0);		//プレイヤーかAIが移動する駒を選択する。modeによってプレイヤーとAIを切り替える

		}

		BoardCheck();			//選択した駒が移動できるマスを保存する



		if (jb > 0 || cb > 0) {	//jbはジャンプできるマスの数、cbは隣に移動できるマスの数（移動できるとき）
			if (logs==0) {
				ShowCBoard();		//移動できるマスを表示する
			}
			SwPlayer(1);		//移動を続けるか選択できる。(1回目の移動をやめることはできない)それから移動するマスを選択する。modeによってプレイヤーかAIが選択する。
			MovePiece();        //駒を移動させる

		}

	} while ((canBoard[selectnX][selectnY] == SUGGESTJ) && mode == 1);	//駒がジャンプしていた場合もう一度行う。ContiTurn()で移動をやめた場合と1マス移動の場合にはwhileを抜ける

	SetBoard(1);		//移動できるマスの情報をリセット
	jumpcount = 0;		//ジャンプ回数をリセット

	WinCheck();			//ボードの状態が勝利条件に合うかチェックする。合致していた場合ゲーム終了

	ChangePlayer();		//プレイヤーを交代する（黒と白）
	//printf("change\n");
	//scanf_s("%d", &test);
}

//ゲームの選択
void Menu() {

	int select = 0, bow = 0;
	//gameend = 0;
	printf("pvp:1 pve:2 aivsai:3\n");
	printf("select:");
	scanf_s("%d", &select);
	//rewind(stdin);
	while (getchar() != '\n');
	switch (select)
	{
	case 1:
		mode = 1;
		break;
	case 2:
		printf("black:1 white:2\n");
		printf("select:");
		scanf_s("%d", &bow);
		//rewind(stdin);
		while (getchar() != '\n');
		if (bow == 1)
		{
			mode = 2;
		}
		else {
			mode = 3;
		}
		break;
	case 3:
		mode = 4;
	default:
		break;
	}

}

//ボードの初期化
void SetBoard(int i) {

	switch (i)
	{
	case(0):
		for (int x = 1; x <= BOARDSIZE; x++)
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (x >= (10 + y) && y <= 5 && x != 11)
				{
					halmaBoard[x][y] = BLACK_STONE;
				}
				else if (y >= (10 + x) && x <= 5 && y != 11) {
					halmaBoard[x][y] = WHITE_STONE;
				}
				else {
					halmaBoard[x][y] = EMPTY;
				}
				halmaBoard[x][0] = WALL;
				halmaBoard[0][y] = WALL;


			}
		}

		break;
	case(1):
		for (int x = 1; x <= BOARDSIZE; x++)
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				canBoard[x][y] = EMPTY;

			}
		}
		jb = 0;
		cb = 0;
		break;
	case(2):
		for (int x = 1; x <= BOARDSIZE; x++)
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				preBoard[x][y] = halmaBoard[x][y];
			}
		}

		break;
	case(3):
		for (int x = 1; x <= BOARDSIZE; x++)
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				mvBoard[x][y] = EMPTY;
			}
		}
		break;
	case(4):
		for (int x = 1; x <= BOARDSIZE; x++)
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				halmaBoard[x][y] = preBoard[x][y];
			}
		}
		break;
	default:
		break;
	}

}

//ボードの表示
void ShowBoard() {
	int x, y;

	printf(" 　　1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 \n");
	for (y = 1; y <= BOARDSIZE; y++)
	{
		if (y < 10) {
			printf("  %d", y);
		}
		else {
			printf(" %d", y);
		}
		for (x = 1; x <= BOARDSIZE; x++)
		{
			if (halmaBoard[x][y] == BLACK_STONE)
				printf(" ○");
			else if (halmaBoard[x][y] == WHITE_STONE)
				printf(" ●");
			else
				printf(" ＊");
		}
		printf("\n");
	}
}

//移動できる場所の表示
void ShowCBoard() {
	int x, y;

	printf(" 　　1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 \n");
	for (y = 1; y <= BOARDSIZE; y++)
	{
		if (y < 10) {
			printf("  %d", y);
		}
		else {
			printf(" %d", y);
		}
		for (x = 1; x <= BOARDSIZE; x++)
		{
			if (canBoard[x][y] == player)
				printf(" ◎");
			else if (halmaBoard[x][y] == BLACK_STONE)
				printf(" ○");
			else if (halmaBoard[x][y] == WHITE_STONE)
				printf(" ●");
			else if (canBoard[x][y] == SUGGEST)
				printf(" ★");
			else if (canBoard[x][y] == SUGGESTJ)
				printf(" ☆");
			else
				printf(" ＊");
		}
		printf("\n");
	}
}

//駒を選択する前にすべての駒を調べる
void PreCheck() {
	for (int x = 1; x <= BOARDSIZE; x++)
	{
		for (int y = 1; y <= BOARDSIZE; y++) {
			selectX = x;
			selectY = y;
			nt = 0;
			BoardCheck();
			SetBoard(1);
		}
	}

}

//駒が移動できるかの確認
void BoardCheck() {
	int ex, ey, py, r;
	for (r = 0; r < 2; r++) {
		ex = 0;
		py = 1;
		ey = 0;
		for (int i = 0; i < 4; i++)
		{
			ey = py;
			if (r == 1) {
				ex *= -1;
				ey *= -1;
			}
			if (selectX + ex <= BOARDSIZE && selectY + ey <= BOARDSIZE && selectX + ex >= 1 && selectY + ey >= 1) {

				if (halmaBoard[selectX][selectY] == player) {

					if (halmaBoard[selectX + ex][selectY + ey] == EMPTY && jumpcount == 0) {
						canBoard[selectX + ex][selectY + ey] = SUGGEST;
						cb++;
					}
					else if (halmaBoard[selectX + ex][selectY + ey] != WALL && halmaBoard[selectX + ex][selectY + ey] != EMPTY) {

						if (selectX + (ex * 2) <= BOARDSIZE && selectY + (ey * 2) <= BOARDSIZE && selectX + (ex * 2) >= 1 && selectY + (ey * 2) >= 1) {

							if (halmaBoard[selectX + (ex * 2)][selectY + (ey * 2)] == EMPTY) {

								if (((selectX + (ex * 2)) == selectpX && (selectY + (ey * 2)) == selectpY) || ((selectX + (ex * 2)) == selectcX && (selectY + (ey * 2)) == selectcY)) {

								}
								else {


									canBoard[selectX + (ex * 2)][selectY + (ey * 2)] = SUGGESTJ;

									jb++;

								}
							}

						}

					}

				}
			}
			ex = 1;
			if (0 < i) {
				py -= 1;

			}

		}

	}
	if ((jb + cb) > 0 && nt == 0) {
		mvBoard[selectX][selectY] = CANMV;

	}

}

//プレイヤーの選択（AIか人か）
void SwPlayer(int i) {
	switch (mode) {
	case(1):
		Select(i);
		break;
	case(2):

		break;
	case(3):
		break;
	case(4):
		if (player == BLACK_STONE) {
			AI_B(i);
		}
		else {
			AI_B(i);
		}
		break;
	}
}

//人の入力
void Select(int i) {

	switch (i)
	{
	case(0):
		if (player == BLACK_STONE)
			printf(" player 〇\n");
		else
			printf(" player ●\n");
		printf(" x:");
		scanf_s("%d", &selectX);	/* 入力部分 */
		printf(" y:");
		scanf_s("%d", &selectY);

		if (halmaBoard[selectX][selectY] != player) {
			printf("#choose your piece!#\n");
			//rewind(stdin);

			Select(0);
		}
		if (mvBoard[selectX][selectY] != CANMV) {
			printf("#can't move!#\n");
			//rewind(stdin);

			Select(0);
		}

		canBoard[selectX][selectY] = player;
		selectpX = selectX;
		selectpY = selectY;

		break;
	case(1):

		while (getchar() != '\n');
		if (jumpcount == 0) {
			Select(2);
		}
		else {

			printf("continue?y/n:");
			scanf_s("%c", &jump, sizeof(jump));
			if (jump[1] == 'y') {
				Select(2);
			}
			else if (jump[1] == 'n') {
				canBoard[selectnX][selectnY] = EMPTY;
				printf("\n");
			}
			else {
				printf("\n");
				printf("#y or n#\n");

				Select(1);
			}
		}
		break;
	case(2):
		if (player == BLACK_STONE)
			printf(" player 〇\n");
		else
			printf(" player ●\n");
		selectcX = selectnX;
		selectcY = selectnY;
		printf(" x:");
		scanf_s("%d", &selectnX);	/* 入力部分 */
		printf(" y:");
		scanf_s("%d", &selectnY);

		if (canBoard[selectnX][selectnY] < 4) {
			printf("#choose a star!#\n");
			//rewind(stdin);

			Select(2);
		}
		break;

	default:
		break;
	}

	//rewind(stdin);

}

//駒の移動処理
void MovePiece() {
	if (canBoard[selectnX][selectnY] == SUGGEST) {
		halmaBoard[selectX][selectY] = EMPTY;
		halmaBoard[selectnX][selectnY] = player;
	}
	else if (canBoard[selectnX][selectnY] == SUGGESTJ) {
		halmaBoard[selectX][selectY] = EMPTY;
		halmaBoard[selectnX][selectnY] = player;
		selectX = selectnX;
		selectY = selectnY;

		jumpcount++;
	}

}

//プレイヤーの交代（黒と白）
void ChangePlayer() {
	if (player == BLACK_STONE && gameend == 0) {
		player = WHITE_STONE;

	}
	else {
		player = BLACK_STONE;
	}

	selectnX = 0;
	selectnY = 0;
	maxsubnumx = 0;
	maxsubnumy = 0;
	presize = 0;
	
}

//勝利判定
void WinCheck() {
	for (int x = 1; x <= BOARDSIZE; x++)
	{
		for (int y = 1; y <= BOARDSIZE; y++) {
			if (x >= (10 + y) && y <= 5 && x != 11 && halmaBoard[x][y] == WHITE_STONE)
			{

				whs++;
			}
			if (y >= (10 + x) && x <= 5 && y != 11 && halmaBoard[x][y] == BLACK_STONE) {
				bls++;

			}

			if (x >= (10 + y) && y <= 5 && x != 11 && halmaBoard[x][y] == EMPTY)
			{

				empw++;
			}
			if (y >= (10 + x) && x <= 5 && y != 11 && halmaBoard[x][y] == EMPTY) {
				empb++;

			}
			if (aiBoard[y][x]>=71&&halmaBoard[x][y]==WHITE_STONE) {
				whsn++;
			}
			if (aiBoard[x][y] >= 71 && halmaBoard[x][y] == BLACK_STONE) {
				blsn++;
			}

		}
	}
	
	//printf("whs%d", whs);
	//printf("bls%d\n", bls);

	if (whsn >= 19 && empw == 0) {
		gameend = 1;
		printf("white win!\n");
		whitewin++;
		if (turnminw>turnnumt) {
			turnminw = turnnumt;
		}
		if(turnmaxw<turnnumt){
			turnmaxw = turnnumt;
		}
		turnavew += turnnumt;
	}
	else if (blsn >= 19 && empb == 0) {
		gameend = 1;
		printf("black win!\n");
		blackwin++;
		if (turnminb > turnnumt) {
			turnminb = turnnumt;
		}
		if (turnmaxb < turnnumt) {
			turnmaxb = turnnumt;
		}
		turnaveb += turnnumt;
	}
	else {
		whs = 0;
		bls = 0;
		empb = 0;
		empw = 0;
		whsn = 0;
		blsn = 0;
	}

	if (gameend == 1) {
		if (logs==1) {
			ShowBoard();
		}
		
		printf("gameover\n");
		//scanf_s("%d", &exitgame);
		//rewind(stdin);
		GameClear();

	}
}

//変数の初期化
void GameClear() {
	cpx.clear();
	cpy.clear();
	cpj0x.clear();
	cpj0y.clear();
	empx.clear();
	empy.clear();
	mvnx.clear();
	mvny.clear();
	selectX = 0;
	selectY = 0;
	selectnX = 0;
	selectnY = 0;
	selectpX = 0;
	selectpY = 0;
	selectcX = 0;
	selectcY = 0;
	cb = 0;
	jb = 0;
	jp = 0;
	whs = 0;
	bls = 0;
	empw = 0;
	empb = 0;
	whsn = 0;
	blsn = 0;

	jumpcount = 0;
	exitgame = 0;
	cmnum = 0;
	test = 0;
	maxsubnumx = 0;
	maxsubnumy = 0;
	nt = 0;
	fn = 0;
	nearx = 0;
	neary = 0;
	slow = 0;
}

//ランダムに駒を選ぶAI
void AI_A(int i) {

	int num = 0;
	int ran = 0;
	int max = -100;
	nearx = 100;
	neary = 100;
	switch (i)
	{
	case(0):
		fn = 0;
		for (int x = 1; x <= BOARDSIZE; x++) {
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (mvBoard[x][y] == CANMV) {
					maxsubnumx = 0;
					maxsubnumy = 0;
					canBoard[x][y] = player;
					selectX = x;
					selectY = y;
					selectpX = selectX;
					selectpY = selectY;
					jumpcount = 0;
					nt = 1;
					BoardCheck();
					nt = 0;
					AI_A(2);
					presize = 0;


					if (player == WHITE_STONE) {
						if (aiBoard[y][x] < aiBoard[maxsubnumy][maxsubnumx]) {
							cpx.push_back(x);
							cpy.push_back(y);
							cmnum++;
							//printf("wx:%d\n", x);
							//printf("wy:%d\n", y);
							//printf("waib:%d\n", aiBoard[y][x]);
							//printf("waimxb:%d\n", aiBoard[maxsubnumy][maxsubnumx]);

						}

					}
					else {
						if (aiBoard[x][y] < aiBoard[maxsubnumx][maxsubnumy]) {
							cpx.push_back(x);
							cpy.push_back(y);
							cmnum++;
							//printf("baib:%d\n", aiBoard[x][y]);
							//printf("baimxb:%d\n", aiBoard[maxsubnumx][maxsubnumy]);
							//scanf_s("%d", &test);
						}
					}


				}
				SetBoard(1);
				//SetBoard(3);
				//PreCheck();
			}
		}

		if (cpx.size() == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (halmaBoard[x][y] == player) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								cpx.push_back(x);
								cpy.push_back(y);
								cmnum++;
								fn = 1;
							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								cpx.push_back(x);
								cpy.push_back(y);
								cmnum++;
								fn = 1;
							}
						}
					}
				}
			}
		}
		//printf("cmnum%d\n",cmnum);
		//printf("cpx%d\n", cpx.size());
		//scanf_s("%d", &test);

		//動かせる駒がないときにマイナス方向でも無理に動かす
		if (cmnum == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (mvBoard[x][y] == CANMV) {
						cpx.push_back(x);
						cpy.push_back(y);
						cmnum++;
					}
				}
			}
		}


		InitRand();

		if (cmnum != 0) {
			ran = (rand() % cmnum);
		}

		selectX = cpx[ran];
		selectY = cpy[ran];
		//scanf_s("%d", &test);

		if (logs==0) {
			
			if (player == BLACK_STONE)
				printf(" player 〇\n");
			else
				printf(" player ●\n");

			printf("x:%d\n", selectX);
			printf("y:%d\n", selectY);
			
		}
		SetBoard(1);
		jumpcount = 0;
		canBoard[selectX][selectY] = player;
		selectpX = selectX;
		selectpY = selectY;

		cmnum = 0;
		cpx.clear();
		cpy.clear();

		break;
	case(1):


		AI_A(2);
		//ShowCBoard();
		if (fn == 1) {
			for (int x = 1; x <= BOARDSIZE; x++)
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (player == WHITE_STONE) {
						if (x >= (10 + y) && y <= 5 && x != 11 && halmaBoard[x][y] == EMPTY)
						{
							empx.push_back(x);
							empy.push_back(y);
							//whs++;
						}
					}
					else {
						if (y >= (10 + x) && x <= 5 && y != 11 && halmaBoard[x][y] == EMPTY) {
							empx.push_back(x);
							empy.push_back(y);
							//bls++;

						}
					}
				}
			}
			for (int x = 1; x <= BOARDSIZE; x++)//71のマスで一番ゴールに近いマス
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (canBoard[x][y] >= 4) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								for (unsigned int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emwhx:%d\n", maxsubnumx);
										//printf("emwhy:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}



							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								for (unsigned int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emblx:%d\n", maxsubnumx);
										//printf("embly:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}
							}
						}
					}

				}
			}
		}
		empx.clear();
		empy.clear();

		selectnX = maxsubnumx;
		selectnY = maxsubnumy;

		//printf("max%d\n", max);

		selectX = selectpX;
		selectY = selectpY;



		break;
	case(2):

		selectcX = selectnX;
		selectcY = selectnY;

		for (int x = 1; x <= BOARDSIZE; x++)//隣のマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGEST) {
					if (player == WHITE_STONE) {
						if (aiBoard[y][x] > max) {
							max = aiBoard[y][x];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("wmax%d\n", max);
						}
					}
					else {
						if (aiBoard[x][y] > max) {
							max = aiBoard[x][y];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("bmax%d\n", max);
						}
					}
				}

			}
		}


		for (int x = 1; x <= BOARDSIZE; x++)//ジャンプするマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGESTJ) {

					cpj0x.push_back(x);
					cpj0y.push_back(y);

				}
			}
		}



		if (cpj0x.size() > 0) {

			for (unsigned int x = 0; x < cpj0x.size(); x++)
			{
				if (player == WHITE_STONE) {
					if (aiBoard[cpj0y[x]][cpj0x[x]] > max) {
						max = aiBoard[cpj0y[x]][cpj0x[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				else
				{
					if (aiBoard[cpj0x[x]][cpj0y[x]] > max) {
						max = aiBoard[cpj0x[x]][cpj0y[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				selectnX = cpj0x[x];
				selectnY = cpj0y[x];

				MovePiece();
				//printf("cpjx+%d\n", cpjx[x]);
				jb = 0;
				cb = 0;
				nt = 1;
				BoardCheck();
				//ShowCBoard();
				//printf("bc\n");
				nt = 0;
				halmaBoard[selectnX][selectnY] = EMPTY;
				halmaBoard[selectpX][selectpY] = player;

				selectnX = selectcX;
				selectnY = selectcY;



			}
			//printf("cpjx:%d\n", cpj0x.size());
			//printf("presize:%d\n", presize);
			if (cpj0x.size() > presize) {

				presize = cpj0x.size();

				cpj0x.clear();
				cpj0y.clear();

				AI_A(2);
			}


		}


		//printf("maxx:%d\n", maxsubnumx);
		//printf("maxy:%d\n", maxsubnumy);
		//printf("\n");

		break;
	default:
		break;
	}


	//rewind(stdin);

	cpj0x.clear();
	cpj0y.clear();
}

//一番後ろの駒を選ぶAI
void AI_B(int i) {

	int num = 0;
	int ran = 0;
	int max = -100;
	nearx = 100;
	neary = 100;
	slow = 100;
	switch (i)
	{
	case(0):
		fn = 0;
		for (int x = 1; x <= BOARDSIZE; x++) {
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (mvBoard[x][y] == CANMV) {
					maxsubnumx = 0;
					maxsubnumy = 0;
					canBoard[x][y] = player;
					selectX = x;
					selectY = y;
					selectpX = selectX;
					selectpY = selectY;
					jumpcount = 0;
					nt = 1;
					BoardCheck();
					nt = 0;
					AI_B(2);
					presize = 0;


					if (player == WHITE_STONE) {
						if (aiBoard[y][x] < aiBoard[maxsubnumy][maxsubnumx]) {
							cpx.push_back(x);
							cpy.push_back(y);
							//cmnum++;
							//printf("wx:%d\n", x);
							//printf("wy:%d\n", y);
							//printf("waib:%d\n", aiBoard[y][x]);
							//printf("waimxb:%d\n", aiBoard[maxsubnumy][maxsubnumx]);

						}

					}
					else {
						if (aiBoard[x][y] < aiBoard[maxsubnumx][maxsubnumy]) {
							cpx.push_back(x);
							cpy.push_back(y);
							//cmnum++;
							//printf("baib:%d\n", aiBoard[x][y]);
							//printf("baimxb:%d\n", aiBoard[maxsubnumx][maxsubnumy]);
							//scanf_s("%d", &test);
						}
					}


				}
				SetBoard(1);
				//SetBoard(3);
				//PreCheck();
			}
		}

		if (cpx.size() == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (halmaBoard[x][y] == player) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								cpx.push_back(x);
								cpy.push_back(y);
								//cmnum++;
								fn = 1;
							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								cpx.push_back(x);
								cpy.push_back(y);
								//cmnum++;
								fn = 1;
							}
						}
					}
				}
			}
		}
		//printf("cmnum%d\n",cmnum);
		//printf("cpx%d\n", cpx.size());
		//scanf_s("%d", &test);
		if (player == WHITE_STONE) {
			for (int t = 0; t < cpx.size(); t++) {
				//printf("aib%d\n", aiBoard[cpy[t]][cpx[t]]);
				if (slow >= aiBoard[cpy[t]][cpx[t]]) {
					slow = aiBoard[cpy[t]][cpx[t]];

				}
			}
			for (int t = 0; t < cpx.size(); t++) {
				if (aiBoard[cpy[t]][cpx[t]] == slow) {
					mvnx.push_back(cpx[t]);
					mvny.push_back(cpy[t]);
					cmnum++;
				}
			}
		}
		else {
			for (int t = 0; t < cpx.size(); t++) {
				//printf("aib%d\n", aiBoard[cpx[t]][cpy[t]]);
				if (slow >= aiBoard[cpx[t]][cpy[t]]) {
					slow = aiBoard[cpx[t]][cpy[t]];

				}
			}
			for (int t = 0; t < cpx.size(); t++) {
				if (aiBoard[cpx[t]][cpy[t]] == slow) {
					mvnx.push_back(cpx[t]);
					mvny.push_back(cpy[t]);
					cmnum++;
				}
			}
		}
		//printf("slow%d\n", slow);
		//printf("cmnum%d\n",cmnum);
		//printf("mvnxsize%d\n", mvnx.size());

		//動かせる駒がないときにマイナス方向でも無理に動かす
		if (cmnum == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (mvBoard[x][y] == CANMV) {
						mvnx.push_back(x);
						mvny.push_back(y);
						cmnum++;
					}
				}
			}
		}

		InitRand();

		if (cmnum != 0) {
			ran = (rand() % cmnum);
		}
		//printf("ran%d\n", ran);
		//scanf_s("%d", &test);

		selectX = mvnx[ran];
		selectY = mvny[ran];
		//scanf_s("%d", &test);

		if (logs==0) {
			if (player == BLACK_STONE)
				printf(" player 〇\n");
			else
				printf(" player ●\n");

			printf("x:%d\n", selectX);
			printf("y:%d\n", selectY);
		}

		SetBoard(1);
		jumpcount = 0;
		canBoard[selectX][selectY] = player;
		selectpX = selectX;
		selectpY = selectY;

		cmnum = 0;
		cpx.clear();
		cpy.clear();
		mvnx.clear();
		mvny.clear();
		break;
	case(1):


		AI_B(2);
		//ShowCBoard();
		if (fn == 1) {
			for (int x = 1; x <= BOARDSIZE; x++)
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (player == WHITE_STONE) {
						if (x >= (10 + y) && y <= 5 && x != 11 && halmaBoard[x][y] == EMPTY)
						{
							empx.push_back(x);
							empy.push_back(y);
							//whs++;
						}
					}
					else {
						if (y >= (10 + x) && x <= 5 && y != 11 && halmaBoard[x][y] == EMPTY) {
							empx.push_back(x);
							empy.push_back(y);
							//bls++;

						}
					}
				}
			}
			for (int x = 1; x <= BOARDSIZE; x++)//71のマスで一番ゴールに近いマス
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (canBoard[x][y] >= 4) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								for (int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emwhx:%d\n", maxsubnumx);
										//printf("emwhy:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}



							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								for (int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emblx:%d\n", maxsubnumx);
										//printf("embly:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}
							}
						}
					}

				}
			}
		}
		empx.clear();
		empy.clear();

		selectnX = maxsubnumx;
		selectnY = maxsubnumy;

		//printf("max%d\n", max);

		selectX = selectpX;
		selectY = selectpY;



		break;
	case(2):

		selectcX = selectnX;
		selectcY = selectnY;

		for (int x = 1; x <= BOARDSIZE; x++)//隣のマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGEST) {
					if (player == WHITE_STONE) {
						if (aiBoard[y][x] > max) {
							max = aiBoard[y][x];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("wmax%d\n", max);
						}
					}
					else {
						if (aiBoard[x][y] > max) {
							max = aiBoard[x][y];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("bmax%d\n", max);
						}
					}
				}

			}
		}


		for (int x = 1; x <= BOARDSIZE; x++)//ジャンプするマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGESTJ) {

					cpj0x.push_back(x);
					cpj0y.push_back(y);

				}
			}
		}



		if (cpj0x.size() > 0) {

			for (unsigned int x = 0; x < cpj0x.size(); x++)
			{
				if (player == WHITE_STONE) {
					if (aiBoard[cpj0y[x]][cpj0x[x]] > max) {
						max = aiBoard[cpj0y[x]][cpj0x[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				else
				{
					if (aiBoard[cpj0x[x]][cpj0y[x]] > max) {
						max = aiBoard[cpj0x[x]][cpj0y[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				selectnX = cpj0x[x];
				selectnY = cpj0y[x];

				MovePiece();
				//printf("cpjx+%d\n", cpjx[x]);
				jb = 0;
				cb = 0;
				nt = 1;
				BoardCheck();
				//ShowCBoard();
				//printf("bc\n");
				nt = 0;
				halmaBoard[selectnX][selectnY] = EMPTY;
				halmaBoard[selectpX][selectpY] = player;

				selectnX = selectcX;
				selectnY = selectcY;



			}
			//printf("cpjx:%d\n", cpj0x.size());
			//printf("presize:%d\n", presize);
			if (cpj0x.size() > presize) {

				presize = cpj0x.size();

				cpj0x.clear();
				cpj0y.clear();

				AI_B(2);
			}


		}


		//printf("maxx:%d\n", maxsubnumx);
		//printf("maxy:%d\n", maxsubnumy);
		//printf("\n");

		break;
	default:
		break;
	}


	//rewind(stdin);

	cpj0x.clear();
	cpj0y.clear();
}

//一番移動距離が大きい駒を選ぶAI
void AI_C(int i) {

	int num = 0;
	int ran = 0;
	int max = -100;
	nearx = 100;
	neary = 100;
	switch (i)
	{
	case(0):
		fn = 0;
		for (int x = 1; x <= BOARDSIZE; x++) {
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (mvBoard[x][y] == CANMV) {
					maxsubnumx = 0;
					maxsubnumy = 0;
					canBoard[x][y] = player;
					selectX = x;
					selectY = y;
					selectpX = selectX;
					selectpY = selectY;
					jumpcount = 0;
					nt = 1;
					BoardCheck();
					nt = 0;
					AI_C(2);
					presize = 0;


					if (player == WHITE_STONE) {
						if (aiBoard[y][x] < aiBoard[maxsubnumy][maxsubnumx]) {
							cpx.push_back(x);
							cpy.push_back(y);
							fary=sqrt(pow(maxsubnumx-x,2)+pow(maxsubnumy-y,2));
							farp.push_back(fary);
							if (far < fary) {
								far = fary;
							}

						}
						/*
						if (aiBoard[y][x] < aiBoard[maxsubnumy][maxsubnumx]) {
							cpx.push_back(x);
							cpy.push_back(y);
							cmnum++;
							//printf("wx:%d\n", x);
							//printf("wy:%d\n", y);
							//printf("waib:%d\n", aiBoard[y][x]);
							//printf("waimxb:%d\n", aiBoard[maxsubnumy][maxsubnumx]);

						}
						*/

					}
					else {
						if (aiBoard[x][y] < aiBoard[maxsubnumx][maxsubnumy]) {
							cpx.push_back(x);
							cpy.push_back(y);
							fary = sqrt(pow(maxsubnumx - x, 2) + pow(maxsubnumy - y, 2));
							farp.push_back(fary);
							if (far < fary) {
								far = fary;
							}
						}                                                          

						/*
						if (aiBoard[x][y] < aiBoard[maxsubnumx][maxsubnumy]) {
							cpx.push_back(x);
							cpy.push_back(y);
							cmnum++;
							//printf("baib:%d\n", aiBoard[x][y]);
							//printf("baimxb:%d\n", aiBoard[maxsubnumx][maxsubnumy]);
							//scanf_s("%d", &test);
						}
						*/
					}


				}
				SetBoard(1);
				//SetBoard(3);
				//PreCheck();
			}
		}

		if (cpx.size() == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (halmaBoard[x][y] == player) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								mvnx.push_back(x);
								mvny.push_back(y);
								cmnum++;
								fn = 1;
							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								mvnx.push_back(x);
								mvny.push_back(y);
								cmnum++;
								fn = 1;
							}
						}
					}
				}
			}
		}
		else {

			for (int t = 0; t < cpx.size(); t++) {
				if (farp[t] == far) {
					mvnx.push_back(cpx[t]);
					mvny.push_back(cpy[t]);
					cmnum++;
				}
			}
		}
		
		//動かせる駒がないときにマイナス方向でも無理に動かす
		if (cmnum == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (mvBoard[x][y] == CANMV) {
						mvnx.push_back(x);
						mvny.push_back(y);
						cmnum++;
					}
				}
			}
		}


		InitRand();

		if (cmnum != 0) {
			ran = (rand() % cmnum);
		}

		selectX = mvnx[ran];
		selectY = mvny[ran];
		//scanf_s("%d", &test);

		if (logs == 0) {

			if (player == BLACK_STONE)
				printf(" player 〇\n");
			else
				printf(" player ●\n");

			printf("x:%d\n", selectX);
			printf("y:%d\n", selectY);

		}
		SetBoard(1);
		jumpcount = 0;
		canBoard[selectX][selectY] = player;
		selectpX = selectX;
		selectpY = selectY;

		cmnum = 0;
		cpx.clear();
		cpy.clear();
		mvnx.clear();
		mvny.clear();
		farp.clear();
		far = 0;
		break;
	case(1):


		AI_C(2);
		//ShowCBoard();
		if (fn == 1) {
			for (int x = 1; x <= BOARDSIZE; x++)
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (player == WHITE_STONE) {
						if (x >= (10 + y) && y <= 5 && x != 11 && halmaBoard[x][y] == EMPTY)
						{
							empx.push_back(x);
							empy.push_back(y);
							//whs++;
						}
					}
					else {
						if (y >= (10 + x) && x <= 5 && y != 11 && halmaBoard[x][y] == EMPTY) {
							empx.push_back(x);
							empy.push_back(y);
							//bls++;

						}
					}
				}
			}
			for (int x = 1; x <= BOARDSIZE; x++)//71のマスで一番ゴールに近いマス
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (canBoard[x][y] >= 4) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								for (unsigned int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emwhx:%d\n", maxsubnumx);
										//printf("emwhy:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}



							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								for (unsigned int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emblx:%d\n", maxsubnumx);
										//printf("embly:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}
							}
						}
					}

				}
			}
		}
		empx.clear();
		empy.clear();

		selectnX = maxsubnumx;
		selectnY = maxsubnumy;

		//printf("max%d\n", max);

		selectX = selectpX;
		selectY = selectpY;



		break;
	case(2):

		selectcX = selectnX;
		selectcY = selectnY;

		for (int x = 1; x <= BOARDSIZE; x++)//隣のマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGEST) {
					if (player == WHITE_STONE) {
						if (aiBoard[y][x] > max) {
							max = aiBoard[y][x];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("wmax%d\n", max);
						}
					}
					else {
						if (aiBoard[x][y] > max) {
							max = aiBoard[x][y];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("bmax%d\n", max);
						}
					}
				}

			}
		}


		for (int x = 1; x <= BOARDSIZE; x++)//ジャンプするマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGESTJ) {

					cpj0x.push_back(x);
					cpj0y.push_back(y);

				}
			}
		}



		if (cpj0x.size() > 0) {

			for (unsigned int x = 0; x < cpj0x.size(); x++)
			{
				if (player == WHITE_STONE) {
					if (aiBoard[cpj0y[x]][cpj0x[x]] > max) {
						max = aiBoard[cpj0y[x]][cpj0x[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				else
				{
					if (aiBoard[cpj0x[x]][cpj0y[x]] > max) {
						max = aiBoard[cpj0x[x]][cpj0y[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				selectnX = cpj0x[x];
				selectnY = cpj0y[x];

				MovePiece();
				//printf("cpjx+%d\n", cpjx[x]);
				jb = 0;
				cb = 0;
				nt = 1;
				BoardCheck();
				//ShowCBoard();
				//printf("bc\n");
				nt = 0;
				halmaBoard[selectnX][selectnY] = EMPTY;
				halmaBoard[selectpX][selectpY] = player;

				selectnX = selectcX;
				selectnY = selectcY;



			}
			//printf("cpjx:%d\n", cpj0x.size());
			//printf("presize:%d\n", presize);
			if (cpj0x.size() > presize) {

				presize = cpj0x.size();

				cpj0x.clear();
				cpj0y.clear();

				AI_C(2);
			}


		}


		//printf("maxx:%d\n", maxsubnumx);
		//printf("maxy:%d\n", maxsubnumy);
		//printf("\n");

		break;
	default:
		break;
	}


	//rewind(stdin);

	cpj0x.clear();
	cpj0y.clear();
}

//ジャンプ可能な駒からランダムに選ぶAI
void AI_D(int i) {

	int num = 0;
	int ran = 0;
	int max = -100;
	nearx = 100;
	neary = 100;
	switch (i)
	{
	case(0):
		fn = 0;
		for (int x = 1; x <= BOARDSIZE; x++) {
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (mvBoard[x][y] == CANMV) {
					maxsubnumx = 0;
					maxsubnumy = 0;
					canBoard[x][y] = player;
					selectX = x;
					selectY = y;
					selectpX = selectX;
					selectpY = selectY;
					jumpcount = 0;
					nt = 1;
					BoardCheck();
					nt = 0;
					AI_D(2);
					presize = 0;


					if (player == WHITE_STONE) {
						if (aiBoard[y][x] < aiBoard[maxsubnumy][maxsubnumx]) {
							if (canBoard[maxsubnumx][maxsubnumy] == SUGGESTJ) {
								sjump.push_back(1);
							}
							else {
								sjump.push_back(0);
							}
							cpx.push_back(x);
							cpy.push_back(y);
							//cmnum++;
							//printf("wx:%d\n", x);
							//printf("wy:%d\n", y);
							//printf("waib:%d\n", aiBoard[y][x]);
							//printf("waimxb:%d\n", aiBoard[maxsubnumy][maxsubnumx]);	
						}
					}
					else {
						if (aiBoard[x][y] < aiBoard[maxsubnumx][maxsubnumy]) {
							if (canBoard[maxsubnumx][maxsubnumy] == SUGGESTJ) {
								sjump.push_back(1);
							}
							else {
								sjump.push_back(0);
							}
							cpx.push_back(x);
							cpy.push_back(y);
							//cmnum++;
							//printf("baib:%d\n", aiBoard[x][y]);
							//printf("baimxb:%d\n", aiBoard[maxsubnumx][maxsubnumy]);
							//scanf_s("%d", &test);
						}
					}
				}
				SetBoard(1);
				//SetBoard(3);
				//PreCheck();
			}
		}

		if (cpx.size() == 0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (halmaBoard[x][y] == player) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								mvnx.push_back(x);
								mvny.push_back(y);
								cmnum++;
								fn = 1;
							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								mvnx.push_back(x);
								mvny.push_back(y);
								cmnum++;
								fn = 1;
							}
						}
					}
				}
			}
		}
		else {

			for (int t = 0; t < cpx.size(); t++) {
				if (sjump[t] == 1) {
					mvnx.push_back(cpx[t]);
					mvny.push_back(cpy[t]);
					cmnum++;
				}
			}
			if (cmnum == 0) {
				for (int t = 0; t < cpx.size(); t++) {
					mvnx.push_back(cpx[t]);
					mvny.push_back(cpy[t]);
					cmnum++;
				}
			}
		}

		//動かせる駒がないときにマイナス方向でも無理に動かす
		if (cmnum==0) {
			for (int x = 1; x <= BOARDSIZE; x++) {
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (mvBoard[x][y] == CANMV) {
						mvnx.push_back(x);
						mvny.push_back(y);
						cmnum++;
					}
				}
			}
		}


		InitRand();

		if (cmnum != 0) {
			ran = (rand() % cmnum);
		}

		selectX = mvnx[ran];
		selectY = mvny[ran];
		//scanf_s("%d", &test);

		if (logs == 0) {

			if (player == BLACK_STONE)
				printf(" player 〇\n");
			else
				printf(" player ●\n");

			printf("x:%d\n", selectX);
			printf("y:%d\n", selectY);

		}
		SetBoard(1);
		jumpcount = 0;
		canBoard[selectX][selectY] = player;
		selectpX = selectX;
		selectpY = selectY;

		cmnum = 0;
		cpx.clear();
		cpy.clear();
		mvnx.clear();
		mvny.clear();
		sjump.clear();
		
		break;
	case(1):


		AI_D(2);
		//ShowCBoard();
		if (fn == 1) {
			for (int x = 1; x <= BOARDSIZE; x++)
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (player == WHITE_STONE) {
						if (x >= (10 + y) && y <= 5 && x != 11 && halmaBoard[x][y] == EMPTY)
						{
							empx.push_back(x);
							empy.push_back(y);
							//whs++;
						}
					}
					else {
						if (y >= (10 + x) && x <= 5 && y != 11 && halmaBoard[x][y] == EMPTY) {
							empx.push_back(x);
							empy.push_back(y);
							//bls++;

						}
					}
				}
			}
			for (int x = 1; x <= BOARDSIZE; x++)//71のマスで一番ゴールに近いマス
			{
				for (int y = 1; y <= BOARDSIZE; y++) {
					if (canBoard[x][y] >= 4) {
						if (player == WHITE_STONE) {
							if (aiBoard[y][x] == 71) {
								for (unsigned int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emwhx:%d\n", maxsubnumx);
										//printf("emwhy:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}



							}
						}
						else {
							if (aiBoard[x][y] == 71) {
								for (unsigned int t = 0; t < empx.size(); t++) {
									if (nearx >= abs(empx[t] - x) && neary >= abs(empy[t] - y)) {
										nearx = abs(empx[t] - x);
										neary = abs(empy[t] - y);
										//p = t;
										maxsubnumx = x;
										maxsubnumy = y;
										//printf("emblx:%d\n", maxsubnumx);
										//printf("embly:%d\n", maxsubnumy);
										//scanf_s("%d", &test);
									}

								}
							}
						}
					}

				}
			}
		}
		empx.clear();
		empy.clear();

		selectnX = maxsubnumx;
		selectnY = maxsubnumy;

		//printf("max%d\n", max);

		selectX = selectpX;
		selectY = selectpY;



		break;
	case(2):

		selectcX = selectnX;
		selectcY = selectnY;

		for (int x = 1; x <= BOARDSIZE; x++)//隣のマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGEST) {
					if (player == WHITE_STONE) {
						if (aiBoard[y][x] > max) {
							max = aiBoard[y][x];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("wmax%d\n", max);
						}
					}
					else {
						if (aiBoard[x][y] > max) {
							max = aiBoard[x][y];
							maxsubnumx = x;
							maxsubnumy = y;
							//printf("bmax%d\n", max);
						}
					}
				}

			}
		}


		for (int x = 1; x <= BOARDSIZE; x++)//ジャンプするマスの情報
		{
			for (int y = 1; y <= BOARDSIZE; y++) {
				if (canBoard[x][y] == SUGGESTJ) {

					cpj0x.push_back(x);
					cpj0y.push_back(y);

				}
			}
		}



		if (cpj0x.size() > 0) {

			for (unsigned int x = 0; x < cpj0x.size(); x++)
			{
				if (player == WHITE_STONE) {
					if (aiBoard[cpj0y[x]][cpj0x[x]] > max) {
						max = aiBoard[cpj0y[x]][cpj0x[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				else
				{
					if (aiBoard[cpj0x[x]][cpj0y[x]] > max) {
						max = aiBoard[cpj0x[x]][cpj0y[x]];
						maxsubnumx = cpj0x[x];
						maxsubnumy = cpj0y[x];

					}
				}
				selectnX = cpj0x[x];
				selectnY = cpj0y[x];

				MovePiece();
				//printf("cpjx+%d\n", cpjx[x]);
				jb = 0;
				cb = 0;
				nt = 1;
				BoardCheck();
				//ShowCBoard();
				//printf("bc\n");
				nt = 0;
				halmaBoard[selectnX][selectnY] = EMPTY;
				halmaBoard[selectpX][selectpY] = player;

				selectnX = selectcX;
				selectnY = selectcY;



			}
			//printf("cpjx:%d\n", cpj0x.size());
			//printf("presize:%d\n", presize);
			if (cpj0x.size() > presize) {

				presize = cpj0x.size();

				cpj0x.clear();
				cpj0y.clear();

				AI_D(2);
			}


		}


		//printf("maxx:%d\n", maxsubnumx);
		//printf("maxy:%d\n", maxsubnumy);
		//printf("\n");

		break;
	default:
		break;
	}


	//rewind(stdin);

	cpj0x.clear();
	cpj0y.clear();
}

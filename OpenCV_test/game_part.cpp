#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "CV_part.h"
#include <graphics.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 480
#define WIN_HEIGHT 800//480*800
#define STEP 7//����
//#define CVSTEP 5//cv����
#define BULLET_NUM 30//�ӵ���
#define ENEMY_NUM 15//�о���
#define THOUS 1000//1000ms=1s

using namespace std;


enum State {
	LIVE, DEATH, BUFF_01, BUFF_02, LIVE_BUFF_01, LIVE_BUFF_02
};
enum Size {
	BIG, SMALL
};
//player���л���live��live buff�ķ���״̬��ͬ


//player��С��л����ڵ�1��2��buff��1��2��
typedef class tag_object {
public:
	int x, y, state, size;
	int hp;
}object;



//����

void IfCollide();
void SizeEnemy(int i);

//����
DWORD t1, t2, t3, t4;
int Game_score = 150;



struct Image
{
	IMAGE Background;
	IMAGE player[4];
	IMAGE bullet[2];
	IMAGE enemy[4];


}image;

object player, bullet[BULLET_NUM], enemy[ENEMY_NUM];



//����

//��ʼ��
void GameInit() {
	//��¼ʱ��
	t1 = t2 = t3 = t4 = GetTickCount();
	srand(GetTickCount());
	//���ر���
	loadimage(&image.Background, L"./image_need/background.png");
	//���
	loadimage(&image.player[0], L"./image_need/planeNormal_1.jpg");
	loadimage(&image.player[1], L"./image_need/planeNormal_2.jpg");
	//�ӵ�
	loadimage(&image.bullet[0], L"./image_need/bullet1.jpg");
	loadimage(&image.bullet[1], L"./image_need/bullet2.jpg");
	//С�о�
	loadimage(&image.enemy[0], L"./image_need/enemy1.jpg");
	loadimage(&image.enemy[1], L"./image_need/enemy2.jpg");
	//��о�
	loadimage(&image.enemy[2], L"./image_need/enemyPlane1.jpg");
	loadimage(&image.enemy[3], L"./image_need/enemyPlane2.jpg");
	
	//loadimage(&image.Background, L"./image_need/background.png");
	//loadimage(&image.Background, L"./image_need/background.png");
	//loadimage(&image.Background, L"./image_need/background.png");

	//��ʼ���������
	player.x = WIN_WIDTH / 2;
	player.y = WIN_HEIGHT - 120;
	player.state = LIVE;


	//��ʼ���ӵ�
	for (int i = 0; i < BULLET_NUM; i++)
	{
		bullet[i].state = DEATH;
	}

}

void CreatBullet() {
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].state == DEATH) {
			bullet[i].state = LIVE;
			bullet[i].x = player.x + 50;
			bullet[i].y = player.y;
			break;
		}
	}

}

void CreatEnemy() {
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].state == DEATH) {
			enemy[i].state = LIVE;
			enemy[i].x = rand() % (WIN_WIDTH - 100);
			enemy[i].y = 0;
			SizeEnemy(i);
			break;
		}
	}
}

void BulletMove() {
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].state == LIVE) {
			bullet[i].y -= STEP;

			if (bullet[i].y <= 0) {
				bullet[i].state = DEATH;
				Game_score -= 2;
			}

		}
	}
}

void EnemyMove() {
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].state == LIVE) {
			enemy[i].y += (rand() % 10 <= 7 ? STEP : 0);

			if (enemy[i].y >= WIN_HEIGHT) {
				enemy[i].state = DEATH;
			}

		}
	}
}

void SizeEnemy(int i) {
	//�л�����
	if (rand() % 10 == 0) {
		enemy[i].size = BIG;
		enemy[i].hp = 3;

	}
	else {
		enemy[i].size = SMALL;
		enemy[i].hp = 1;

	}

}

void IfCollide() {

	for (int j = 0; j < ENEMY_NUM; j++)
		if (enemy[j].state == LIVE)
		{
			for (int i = 0; i < BULLET_NUM; i++)
			{
				if (bullet[i].state == LIVE &&
					bullet[i].x > enemy[j].x + 19 &&
					bullet[i].x < enemy[j].x + 32 + 19) {
					if (bullet[i].y <= enemy[j].y + 41) {
						bullet[i].state = DEATH;
						enemy[j].state = DEATH;
						Game_score += 5;
					}

				}
			}

		}



}

//���ÿһ֡
void GameDraw() {

	//����
	putimage(0, 0, &image.Background);

	//���
	putimage(player.x, player.y, &image.player[0], NOTSRCERASE);
	putimage(player.x, player.y, &image.player[1], SRCINVERT);
	//�л�
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].state == LIVE && enemy[i].size == SMALL) {
			putimage(enemy[i].x, enemy[i].y, &image.enemy[0], NOTSRCERASE);
			putimage(enemy[i].x, enemy[i].y, &image.enemy[1], SRCINVERT);

		}
		if (enemy[i].state == LIVE && enemy[i].size == BIG) {
			putimage(enemy[i].x, enemy[i].y, &image.enemy[2], NOTSRCERASE);
			putimage(enemy[i].x, enemy[i].y, &image.enemy[3], SRCINVERT);

		}
	}


	//�����ӵ�
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].state == LIVE) {
			putimage(bullet[i].x, bullet[i].y, &image.bullet[0], NOTSRCERASE);
			putimage(bullet[i].x, bullet[i].y, &image.bullet[1], SRCINVERT);
		}
	}

	//char str[10]; 
	//sprintf(str, "%d", Game_score);
	//outtextxy(0, 0, str);
	//outtext(0, 0, Game_score);
	//CT_PutNumber(0, 0, Game_score);
	TCHAR s[5];
	_stprintf(s, _T("%d"), Game_score);		// �߰汾 VC �Ƽ�ʹ�� _stprintf_s ����
	outtextxy(0, 0, s);

}

void gamecontrol() {

	if (GetAsyncKeyState(VK_UP) && player.y >= 0) {//��
		player.y -= STEP;
	}
	if (GetAsyncKeyState(VK_DOWN) && player.y + 120 <= WIN_HEIGHT) {//��
		player.y += STEP;
	}
	if (GetAsyncKeyState(VK_LEFT) && player.x >= -58) {//��
		player.x -= STEP;
	}
	if (GetAsyncKeyState(VK_RIGHT) && player.x + 58 <= WIN_WIDTH) {//��
		player.x += STEP;
	}
	//�ӿ�
	deliver_x *= 3;
	deliver_y *= 3;

	if (deliver_y > 0 && player.y >= 0) {//��
		//��Ŀ��������ڷ�Χ����?  ��,�������ƶ�������,������ ;��ͬ
		player.y = player.y - deliver_y >= 0 ? player.y - deliver_y : 0;
	}
	if (deliver_y < 0 && player.y + 120 <= WIN_HEIGHT) {//��
		player.y = player.y - deliver_y <= WIN_HEIGHT - 120 ? player.y - deliver_y : WIN_HEIGHT - 120;
	}
	if (deliver_x < 0 && player.x >= -58) {//��
		player.x = player.x + deliver_x >= -57 ? player.x + deliver_x : -57;
	}
	if (deliver_x > 0 && player.x + 58 <= WIN_WIDTH) {//��
		player.x = player.x + deliver_x <= WIN_WIDTH - 56 ? player.x + deliver_x : WIN_WIDTH - 56;
	}
	cout << "(" << deliver_x << "," << deliver_y << ")" << endl;
	deliver_x = deliver_y = 0;



	if ( rand() % 10 <=8 ) {

		CreatBullet();
		//t1 = t2;
	}
	BulletMove();

	t2 = GetTickCount();
	if (rand() % 100 <= 1 && t2 - t1 < 1 * THOUS) {//һ���ӣ��Ӷ�л�
		CreatEnemy();
	}
	else if (rand() % 100 <= 20) {
		CreatEnemy();
	}

	//if (t4 - t3 > rand() % 500 + 100) {
	//	t3 = t4;
	//	CreatEnemy();
	//}
	//t4 = GetTickCount();
	IfCollide();
	EnemyMove();



	if (Game_score <= 0) {

		exit(100);
	}

}




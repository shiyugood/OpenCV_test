#pragma once
#ifndef GAME_PART
#define GAME_PART
#include <iostream>
#include <graphics.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define WIN_WIDTH 480
#define WIN_HEIGHT 800//480*800
#define STEP 1//²½Êý
#define BULLET_NUM 30
#define ENEMY_NUM 15
#define THOUS 1000//1000ms=1s

using namespace std;

void GameInit();
void CreatBullet();
void CreatEnemy();
void BulletMove();
void EnemyMove();
void IfCollide();
void SizeEnemy(int i);
void GameDraw();
void gamecontrol();


#endif // !GAME_PART

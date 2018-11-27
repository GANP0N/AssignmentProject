/*
=================
- cEnemy.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#pragma once
#include "cSprite.h"
#include <random>

class cEnemy :	public cSprite
{
private:
	MapRC enemyPos;
	MapRC enemy2Pos;
	MapRC enemy3Pos;
	float enemyRotation;
	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.
	uniform_int_distribution<> spriteRandom{ 0, 9 };

public:
	cEnemy();
	virtual ~cEnemy();
	void update(int row, int column);
	void setMapPosition(int row, int column);
	MapRC getMapPosition();
	void setEnemyRotation(float angle);
	float getEnemyRotation();
	void genRandomPos(int row, int column);
};


#include "cEnemy.h"


/*
=================================================================
Defualt Constructor
=================================================================
*/
cEnemy::cEnemy()
{
	this->setMapPosition(this->spriteRandom(gen), this->spriteRandom(gen));
}
void cEnemy::update(int row, int column)
{
	this->setMapPosition(column, row);
}

void cEnemy::setMapPosition(int row, int column)
{
	this->enemyPos = { column, row };
}

MapRC cEnemy::getMapPosition()
{
	return this->enemyPos;
}

void cEnemy::setEnemyRotation(float angle)
{
	this->enemyRotation = angle;
	this->setSpriteRotAngle(angle);
}

float cEnemy::getEnemyRotation()
{
	return this->enemyRotation;
}

void cEnemy::genRandomPos(int row, int column)
{

	while (this->getMapPosition().C == column && this->getMapPosition().R == row)
	{
		this->setMapPosition(this->spriteRandom(gen), this->spriteRandom(gen));
	}
}

/*
=================================================================
Defualt Destructor
=================================================================
*/


cEnemy::~cEnemy()
{
}

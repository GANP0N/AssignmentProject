/*
=================
- cPlayer.cpp
- Header file for class definition - IMPLEMENTATION
=================
*/
#include "cPlayer.h"


/*
=================================================================
Defualt Constructor
=================================================================
*/
cPlayer::cPlayer()
{
	this->setMapPosition( 0, 0 );
}

void cPlayer::update(int row, int column)
{
	this->setMapPosition(column, row );
}

void cPlayer::setMapPosition(int row, int column)
{
	this->playerPos = { column, row };
}

MapRC cPlayer::getMapPosition()
{
	return this->playerPos;
}

void cPlayer::setPlayerRotation(float angle)
{
	this->playerRotation = angle;
	this->setSpriteRotAngle(angle);
}

float cPlayer::getPlayerRotation()
{
	return this->playerRotation;
}



/*
=================================================================
Defualt Destructor
=================================================================
*/
cPlayer::~cPlayer()
{
}

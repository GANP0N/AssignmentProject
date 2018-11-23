#pragma once
#include "cSprite.h"
class cPlayer : public cSprite
{
private:
	MapRC playerPos;
	float playerRotation;
public:
	cPlayer();
	virtual ~cPlayer();
	void update(int row, int column);
	void setMapPosition(int row, int column);
	MapRC getMapPosition();
	void setPlayerRotation(float angle);
	float getPlayerRotation();
	bool  isStomping;
};


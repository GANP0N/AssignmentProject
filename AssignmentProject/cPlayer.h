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
	bool isStomping;
	bool stompTimer1; //Timer until initialisation of the stomp
	bool stompTimer2; //Timer until the end of the stomp
	int stompTime1; //How long the stomp takes to startup
	int stompTime2; //How long the stomp should stay active for
	bool isCaught = false;
};


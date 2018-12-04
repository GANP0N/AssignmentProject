/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"


cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	/* Let the computer pick a random number */
	random_device rd;    // non-deterministic engine 
	mt19937 gen{ rd() }; // deterministic engine. For most common uses, std::mersenne_twister_engine, fast and high-quality.
	uniform_int_distribution<> spriteRandom{ 0, 9 };
	

	theAreaClicked = { 0, 0 };
	// Store the textures
	textureName = { "sky", "enemy", "player", "playerAttack1", "playerAttack2", "theBackground", "OpeningScreen", "ClosingScreen"};
	texturesToUse = { "Images/Sprites/sky64x64.png", "Images/Sprites/Enemy.png", "Images/Sprites/darkChar.png", "Images/Sprites/darkCharAttack1.png", "Images/Sprites/darkCharAttack2.png","Images/Bkg/Bkgnd.jpg", "Images/Bkg/OpeningScreenF.jpg", "Images/Bkg/ClosingScreenF.jpg" };
	for (unsigned int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	tempTextTexture = theTextureMgr->getTexture("sky");
	aRect = { 0, 0, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	aColour = { 228, 213, 238, 255 };
	// Store the textures
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" };
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png", "Images/Buttons/button_settings.png" };
	btnPos = { { 400, 375 }, { 400, 300 }, { 400, 300 }, { 500, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } };
	for (unsigned int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (unsigned int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	theGameState = gameState::menu;
	theBtnType = btnTypes::exit;
	// Create textures for Game Dialogue (text)
	fontList = { "germania", "truelies" };
	fontsToUse = { "Fonts/Germania.ttf", "Fonts/TrueLies.ttf" };
	for (unsigned int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 48);
	}
	// Create text Textures
	gameTextNames = { "TitleTxt", "CollectTxt", "InstructTxt", "InstructTxt2", "GameOverTxt", "SeeYouTxt", "Missed", "MissedOutOf", "Score", "enemyCount", "chanceCount"};
	gameTextList = { "STOMP", "Defeat the Robots when they're 1 tile below you!", "Use the Left & Right arrow keys to move.", "Use the SPACEBAR to Stomp!", "Game Over! You've Been Caught!", "See you next time!", "Missed: ", "/3", "Score: ", "Score: ", ""};
	for (unsigned int text = 0; text < gameTextNames.size(); text++)
	{
		if (text == 0)
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("truelies")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 175, 0, 0, 255 }, { 0, 0, 0, 0 }));
		}
		else
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("germania")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 0, 0, 90, 255 }, { 0, 0, 0, 0 }));
		}
	}
	// Load game sounds
	soundList = { "mainTheme", "wind", "click", "stomp", "stompHit", "roboOut", "move"};
	soundTypes = { soundType::music, soundType::music, soundType::sfx, soundType::sfx, soundType::sfx, soundType::sfx , soundType::sfx};
	soundsToUse = { "Audio/Theme/FastAce.wav", "Audio/Theme/Wind.wav", "Audio/SFX/ClickOn.wav", "Audio/SFX/Stomp.wav", "Audio/SFX/StompHit.wav", "Audio/SFX/roboOut.wav", "Audio/SFX/move.wav" };
	for (unsigned int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("OpeningScreen"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("OpeningScreen")->getTWidth(), theTextureMgr->getTexture("OpeningScreen")->getTHeight());

	theTileMap.setMapStartXY({ 310, 100 });
	thePlayer.setMapPosition(0, 1);
	theEnemy.setMapPosition(8, 2);
	theEnemy2.setMapPosition(8, 1);
	theEnemy3.setMapPosition(8, 0);
	theTileMap.update(thePlayer.getMapPosition(), 3, thePlayer.getPlayerRotation());
	theTileMap.update(theEnemy.getMapPosition(), 2, theEnemy.getEnemyRotation());
	theTileMap.update(theEnemy2.getMapPosition(), 2, theEnemy2.getEnemyRotation());
	theTileMap.update(theEnemy3.getMapPosition(), 2, theEnemy3.getEnemyRotation());

	Score = 0;
	strScore = gameTextList[gameTextList.size() - 1];
	strScore += to_string(Score).c_str();
	theTextureMgr->deleteTexture("enemyCount");

	Chance = 0;
	strChance = gameTextList[gameTextList.size() - 1];
	strChance += to_string(Chance).c_str();
	theTextureMgr->deleteTexture("chanceCount");

//	theSoundMgr->getSnd("mainTheme")->play(-1);

	
}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
	case gameState::menu:
	{
		
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("OpeningScreen"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("OpeningScreen")->getTWidth(), theTextureMgr->getTexture("OpeningScreen")->getTHeight());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 300, 25, tempTextTexture->getTextureRect().w + 200, tempTextTexture->getTextureRect().h + 100 };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CollectTxt");
		pos = { 50, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("InstructTxt");
		pos = { 50, 275, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("InstructTxt2");
		pos = { 50, 400, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		// Render Button
		theButtonMgr->getBtn("play_btn")->setSpritePos({ 300, 600 });
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 600, 600 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	
		
	}
	break;
	case gameState::playing:
	{
		
	
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("OpeningScreen")->getTHeight());
		
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 400, 20, tempTextTexture->getTextureRect().w + 30, tempTextTexture->getTextureRect().h + 15 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("Missed");
		pos = { 100, 20, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h};
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 700, 20, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h};
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("MissedOutOf");
		pos = { 275, 20, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		theTextureMgr->addTexture("enemyCount", theFontMgr->getFont("germania")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 95, 0, 0, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("enemyCount");
		pos = { 825, 20, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		
		theTextureMgr->addTexture("chanceCount", theFontMgr->getFont("germania")->createTextTexture(theRenderer, strChance.c_str(), textType::solid, { 0, 0, 95, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("chanceCount");
		pos = { 250, 20, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theTileMap.render(theSDLWND, theRenderer, theTextureMgr, textureName);
		theTileMap.renderGridLines(theRenderer, aRect, aColour);
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 850, 600 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	
	break;
	case gameState::end:
	{

		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("ClosingScreen"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("ClosingScreen")->getTWidth(), theTextureMgr->getTexture("OpeningScreen")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 400, 20, tempTextTexture->getTextureRect().w + 30, tempTextTexture->getTextureRect().h + 15};
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 320, 275, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);


		tempTextTexture = theTextureMgr->getTexture("Score");
		pos = { 400, 400, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		theTextureMgr->addTexture("enemyCount", theFontMgr->getFont("germania")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 95, 0, 0, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("enemyCount");
		pos = { 525, 400, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		if (thePlayer.isCaught == true)
		{
			tempTextTexture = theTextureMgr->getTexture("GameOverTxt");
			pos = { 200, 200, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
			tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		}

		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 450, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 458, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	
	break;
	case gameState::quit:
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{
	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{
	
}

void cGame::update(double deltaTime)
{
	//Measures set passed frames as a form of timer
	if (theGameState == gameState::playing)
	{
		cout << "time elapsed: " << enemyTimerMedi << endl;
		enemyTimerSlow++;
		enemyTimerMedi++;
		enemyTimerFast++;
		
		if (enemyTimerSlow > 48)
		{
			enemyTimerSlow = 0;
		}

		if (enemyTimerMedi > 36)
		{
			enemyTimerMedi = 0;
		}

		if (enemyTimerFast > 25)
		{
			enemyTimerFast = 0;
		}
	}

	//Checks to see if Enemy has reached the player
	if (theEnemy.getMapPosition().R == thePlayer.getMapPosition().R)
	{
		Chance++;
		theSoundMgr->getSnd("roboOut")->play(0);
		strChance = gameTextList[gameTextList.size() - 1];
		strChance += to_string(Chance).c_str();
		theTextureMgr->deleteTexture("chanceCount");
		if (theEnemy.getMapPosition() == thePlayer.getMapPosition())
		{
			theTileMap.update(theEnemy.getMapPosition(), 3, theEnemy.getEnemyRotation());
			theEnemy.setMapPosition(8, 2);
			theTileMap.update(theEnemy.getMapPosition(), 2, theEnemy.getEnemyRotation());
		}
		else
		{
			theTileMap.update(theEnemy.getMapPosition(), 1, theEnemy.getEnemyRotation());
			theEnemy.setMapPosition(8, 2);
			theTileMap.update(theEnemy.getMapPosition(), 2, theEnemy.getEnemyRotation());
		}
	}


	//Checks to see if Enemy2 has reached the player
	if (theEnemy2.getMapPosition().R == thePlayer.getMapPosition().R)
	{
		Chance++;
		theSoundMgr->getSnd("roboOut")->play(0);
		strChance = gameTextList[gameTextList.size() - 1];
		strChance += to_string(Chance).c_str();
		theTextureMgr->deleteTexture("chanceCount"); 
		if (theEnemy2.getMapPosition() == thePlayer.getMapPosition())
		{
			theTileMap.update(theEnemy2.getMapPosition(), 3, theEnemy2.getEnemyRotation());
			theEnemy2.setMapPosition(8, 1);
			theTileMap.update(theEnemy2.getMapPosition(), 2, theEnemy2.getEnemyRotation());
		}
		else
		{
			theTileMap.update(theEnemy2.getMapPosition(), 1, theEnemy2.getEnemyRotation());
			theEnemy2.setMapPosition(8, 1);
			theTileMap.update(theEnemy2.getMapPosition(), 2, theEnemy2.getEnemyRotation());
		}
	}

	//Checks to see if Enemy3 has reached the player
	if (theEnemy3.getMapPosition().R == thePlayer.getMapPosition().R)
	{
		Chance++;
		theSoundMgr->getSnd("roboOut")->play(0);
		strChance = gameTextList[gameTextList.size() - 1];
		strChance += to_string(Chance).c_str();
		theTextureMgr->deleteTexture("chanceCount");
		if (theEnemy3.getMapPosition() == thePlayer.getMapPosition())
		{
			theTileMap.update(theEnemy3.getMapPosition(), 3, theEnemy3.getEnemyRotation());
			theEnemy3.setMapPosition(8, 0);
			theTileMap.update(theEnemy3.getMapPosition(), 2, theEnemy3.getEnemyRotation());
		}

		else
		{
			theTileMap.update(theEnemy3.getMapPosition(), 1, theEnemy3.getEnemyRotation());
			theEnemy3.setMapPosition(8, 0);
			theTileMap.update(theEnemy3.getMapPosition(), 2, theEnemy3.getEnemyRotation());
		}
	}


	if (thePlayer.stompTimer1 == true)
	{
		thePlayer.stompTime1++;
		thePlayer.stompTime2++;
		if (thePlayer.stompTime1 == 16)
		{
			theTileMap.update(thePlayer.getMapPosition(), 5, thePlayer.getPlayerRotation());
			thePlayer.isStomping = true;
			thePlayer.stompTimer2 = true;
			theSoundMgr->getSnd("stomp")->play(0);
			
			
		}

		cout << "time elapsed: " << thePlayer.stompTime2 << endl;
		if (thePlayer.stompTime2 == 30)
		{
			theTileMap.update(thePlayer.getMapPosition(), 3, thePlayer.getPlayerRotation());
			thePlayer.isStomping = false;
			thePlayer.stompTime1 = 0;
			thePlayer.stompTime2 = 0;
			thePlayer.stompTimer1 = false;
			thePlayer.stompTimer2 = false;
		}

	}


	// Check Button clicked and change state
	if (theGameState == gameState::menu || theGameState == gameState::end)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, gameState::quit, theAreaClicked);
	}

	else
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, gameState::end, theAreaClicked);
	}

	theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, gameState::playing, theAreaClicked);
	theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, gameState::menu, theAreaClicked);

	if (Chance == 3)
	{
		thePlayer.isCaught = true;
		theAreaClicked.x = 860;
		theAreaClicked.y = 610;
	}
	else {
		thePlayer.isCaught = false;
	}

	if (theGameState == gameState::menu)
		if (thePlayer.isCaught = true)
		{
			Score = 0;
			Chance = 0;

			theTileMap.update(thePlayer.getMapPosition(), 1, thePlayer.getPlayerRotation());
			thePlayer.setMapPosition(0, 1);
			theTileMap.update(thePlayer.getMapPosition(), 3, thePlayer.getPlayerRotation());

			theTileMap.update(theEnemy.getMapPosition(), 1, theEnemy.getEnemyRotation());
			theEnemy.setMapPosition(8, 2);
			theTileMap.update(theEnemy.getMapPosition(), 2, theEnemy.getEnemyRotation());

			theTileMap.update(theEnemy2.getMapPosition(), 1, theEnemy2.getEnemyRotation());
			theEnemy2.setMapPosition(8, 1);
			theTileMap.update(theEnemy2.getMapPosition(), 2, theEnemy2.getEnemyRotation());

			theTileMap.update(theEnemy3.getMapPosition(), 1, theEnemy3.getEnemyRotation());
			theEnemy3.setMapPosition(8, 0);
			theTileMap.update(theEnemy3.getMapPosition(), 2, theEnemy3.getEnemyRotation());
			
			thePlayer.isCaught = false;
		}

	// Check if Player has collided with Enemy
	if (thePlayer.isStomping == true)
	{
		if ((thePlayer.getMapPosition().C == theEnemy.getMapPosition().C) && thePlayer.getMapPosition().R == (theEnemy.getMapPosition().R) - 1)
		{
			Score++;
			theSoundMgr->getSnd("stompHit")->play(0);
			strScore = gameTextList[gameTextList.size() - 1];
			strScore += to_string(Score).c_str();
			theTextureMgr->deleteTexture("enemyCount");
			theTileMap.update(theEnemy.getMapPosition(), 1, theEnemy.getEnemyRotation());
			theEnemy.setMapPosition(8, 2);
			theTileMap.update(theEnemy.getMapPosition(), 2, theEnemy.getEnemyRotation());
		}
	}

	// Check if Player has collided with Enemy2
	if (thePlayer.isStomping == true)
	{
		if ((thePlayer.getMapPosition().C == theEnemy2.getMapPosition().C) && thePlayer.getMapPosition().R == (theEnemy2.getMapPosition().R) - 1)
		{
			Score++;
			theSoundMgr->getSnd("stompHit")->play(0);
			strScore = gameTextList[gameTextList.size() - 1];
			strScore += to_string(Score).c_str();
			theTextureMgr->deleteTexture("enemyCount");
			theTileMap.update(theEnemy2.getMapPosition(), 1, theEnemy2.getEnemyRotation());
			theEnemy2.setMapPosition(8, 1);
			theTileMap.update(theEnemy2.getMapPosition(), 2, theEnemy2.getEnemyRotation());
		}
	}

	// Check if Player has collided with Enemy3
	if (thePlayer.isStomping == true)
	{
		if ((thePlayer.getMapPosition().C == theEnemy3.getMapPosition().C) && thePlayer.getMapPosition().R == (theEnemy3.getMapPosition().R) - 1)
		{
			Score++;
			theSoundMgr->getSnd("stompHit")->play(0);
			strScore = gameTextList[gameTextList.size() - 1];
			strScore += to_string(Score).c_str();
			theTextureMgr->deleteTexture("enemyCount");
			theTileMap.update(theEnemy3.getMapPosition(), 1, theEnemy3.getEnemyRotation());
			theEnemy3.setMapPosition(8, 0);
			theTileMap.update(theEnemy3.getMapPosition(), 2, theEnemy3.getEnemyRotation());
		}
	}



	if (theGameState == gameState::playing)
	{

		if (enemyTimerSlow == 48)
		{
			theTileMap.updateEnemy(theEnemy.getMapPosition(), 1);
			theEnemy.setMapPosition(theEnemy.getMapPosition().R - 1, theEnemy.getMapPosition().C);
			theTileMap.updateEnemy(theEnemy.getMapPosition(), 2);
		}

		if (enemyTimerMedi == 36)
		{
			theTileMap.updateEnemy2(theEnemy2.getMapPosition(), 1);
			theEnemy2.setMapPosition(theEnemy2.getMapPosition().R - 1, theEnemy2.getMapPosition().C);
			theTileMap.updateEnemy2(theEnemy2.getMapPosition(), 2);
		}

		if (enemyTimerFast == 25)
		{
			theTileMap.updateEnemy3(theEnemy3.getMapPosition(), 1);
			theEnemy3.setMapPosition(theEnemy3.getMapPosition().R - 1, theEnemy3.getMapPosition().C);
			theTileMap.updateEnemy3(theEnemy3.getMapPosition(), 2);
		}
	}

/*	if (thePlayer.isCaught == true)
	{
		if (theGameState == gameState::end)
		{
			cout << "Pointer points to position: " << theButtonMgr->getBtn("menu_btn")->getClicked();
			if (theButtonMgr->getBtn("menu_btn")->getClicked() == 1)
			{
				theGameState == gameState::menu;
			}
		}
	}*/

}


bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
					//if (theGameState == gameState::playing)
					//{
					//	theTilePicker.update(theAreaClicked);
					//	if (theTilePicker.getTilePicked() > -1)
					//	{
					//		dragTile.setSpritePos(theAreaClicked);
					//		dragTile.setTexture(theTextureMgr->getTexture(textureName[theTilePicker.getTilePicked() - 1]));
					//		dragTile.setSpriteDimensions(theTextureMgr->getTexture(textureName[theTilePicker.getTilePicked() - 1])->getTWidth(), theTextureMgr->getTexture(textureName[theTilePicker.getTilePicked() - 1])->getTHeight());
					//	}
					//}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					//if (theGameState == gameState::playing)
					//{
					//	theAreaClicked = { event.motion.x, event.motion.y };
					//	theTileMap.update(theAreaClicked, theTilePicker.getTilePicked());
					//	theTilePicker.setTilePicked(-1);
					//}
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			{
				dragTile.setSpritePos({ event.motion.x, event.motion.y });
			}
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;

				case SDLK_RIGHT:
				{
					if (theGameState == gameState::playing)
					{
						if (thePlayer.getMapPosition().C < 2)
						{
							theSoundMgr->getSnd("move")->play(0);
							theTileMap.update(thePlayer.getMapPosition(), 1, thePlayer.getPlayerRotation());
							thePlayer.setMapPosition(thePlayer.getMapPosition().R, thePlayer.getMapPosition().C + 1);
							thePlayer.setPlayerRotation(0);
							theTileMap.update(thePlayer.getMapPosition(), 3, thePlayer.getPlayerRotation());
						}
					}
				}
				break;
				
				case SDLK_LEFT:
				{
					if (theGameState == gameState::playing)
					{
						if (thePlayer.getMapPosition().C > 0)
						{
							theSoundMgr->getSnd("move")->play(0);
							theTileMap.update(thePlayer.getMapPosition(), 1, thePlayer.getPlayerRotation());
							thePlayer.setMapPosition(thePlayer.getMapPosition().R, thePlayer.getMapPosition().C - 1);
							thePlayer.setPlayerRotation(0);
							theTileMap.update(thePlayer.getMapPosition(), 3, thePlayer.getPlayerRotation());
						}
					}
				}
				break;
				case SDLK_SPACE:
				{
					if (theGameState == gameState::playing)
					{

						if (thePlayer.getMapPosition().C >= 0)
						{
							//if (thePlayer.isStomping = false)
							//{
								theTileMap.update(thePlayer.getMapPosition(), 4, thePlayer.getPlayerRotation());
								thePlayer.stompTimer1 = true;

							//}
						}
						else
						{
							thePlayer.isStomping = false;
							theTileMap.update(thePlayer.getMapPosition(), 3, thePlayer.getPlayerRotation());
						}
					}
					
				}

				break;
				case SDLK_RETURN:
				{
					
					
						theGameState = gameState::menu;
					
				}


				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	//Quit FONT system
	TTF_Quit();

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}


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
	textureName = { "sea", "bottle", "ship","theBackground", "OpeningScreen", "ClosingScreen"};
	texturesToUse = { "Images/Sprites/sea64x64.png", "Images/Sprites/Bottle64x64.png", "Images/Sprites/shipGreen64x64.png", "Images/Bkg/Bkgnd.png", "Images/Bkg/OpeningScreenF.png", "Images/Bkg/ClosingScreenF.png" };
	for (unsigned int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	tempTextTexture = theTextureMgr->getTexture("sea");
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
	fontList = { "pirate", "skeleton" };
	fontsToUse = { "Fonts/BlackPearl.ttf", "Fonts/SkeletonCloset.ttf" };
	for (unsigned int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 48);
	}
	// Create text Textures
	gameTextNames = { "TitleTxt", "CollectTxt", "InstructTxt", "ThanksTxt", "SeeYouTxt","BottleCount"};
	gameTextList = { "Stomp", "Defeat the enemies!", "Use the Left & Right arrow keys to move.", "Thanks for playing!", "See you again soon!", "Collected: "};
	for (unsigned int text = 0; text < gameTextNames.size(); text++)
	{
		if (text == 0)
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("pirate")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 44, 203, 112, 255 }, { 0, 0, 0, 0 }));
		}
		else
		{
			theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("skeleton")->createTextTexture(theRenderer, gameTextList[text], textType::solid, { 44, 203, 112, 255 }, { 0, 0, 0, 0 }));
		}
	}
	// Load game sounds
	soundList = { "theme", "click" };
	soundTypes = { soundType::music, soundType::sfx};
	soundsToUse = { "Audio/Theme/Kevin_MacLeod_-_Winter_Reflections.wav", "Audio/SFX/ClickOn.wav"};
	for (unsigned int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1);

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("OpeningScreen"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("OpeningScreen")->getTWidth(), theTextureMgr->getTexture("OpeningScreen")->getTHeight());

	theTileMap.setMapStartXY({ 150, 100 });
	theShip.setMapPosition(0, 1);
	theBottle.setMapPosition(1, 2);
	theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
	theTileMap.update(theBottle.getMapPosition(), 2, theBottle.getBottleRotation());

	bottlesCollected = 0;
	strScore = gameTextList[gameTextList.size() - 1];
	strScore += to_string(bottlesCollected).c_str();
	theTextureMgr->deleteTexture("BottleCount");
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
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CollectTxt");
		pos = { 50, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("InstructTxt");
		pos = { 50, 175, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Button
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 });
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
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theTextureMgr->addTexture("BottleCount", theFontMgr->getFont("pirate")->createTextTexture(theRenderer, strScore.c_str(), textType::solid, { 44, 203, 112, 255 }, { 0, 0, 0, 0 }));
		tempTextTexture = theTextureMgr->getTexture("BottleCount");
		pos = { 600, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
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
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 50, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 50, 175, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
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
	// CHeck Button clicked and change state
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

	// Check if ship has collided with the bottle
	if ((theShip.getMapPosition().C == theBottle.getMapPosition().C) && theShip.getMapPosition().R == (theBottle.getMapPosition().R)-1)
	{
		if (theShip.isStomping == true) {
			bottlesCollected++;
			theSoundMgr->getSnd("click")->play(0);
			strScore = gameTextList[gameTextList.size() - 1];
			strScore += to_string(bottlesCollected).c_str();
			theTextureMgr->deleteTexture("BottleCount");
			theBottle.genRandomPos(theShip.getMapPosition().R, theShip.getMapPosition().C);
			theTileMap.update(theBottle.getMapPosition(), 2, theBottle.getBottleRotation());
			theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
			theShip.isStomping = false;
			
		}
	}
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
				/*case SDLK_DOWN:
				{
						if(theGameState == gameState::playing)
							if (theShip.getMapPosition().R < 9)
							{
								theTileMap.update(theShip.getMapPosition(), 1, theShip.getShipRotation());
								theShip.setMapPosition(theShip.getMapPosition().R + 1, theShip.getMapPosition().C);
								theShip.setShipRotation(0);
								theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
							}
				}
				break;
				
				case SDLK_UP:
				{
					if (theGameState == gameState::playing)
						if (theShip.getMapPosition().R > 0)
						{
							theTileMap.update(theShip.getMapPosition(), 1, theShip.getShipRotation());
							theShip.setMapPosition(theShip.getMapPosition().R - 1, theShip.getMapPosition().C);
							theShip.setShipRotation(180);
							theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
						}
				}
				break;
				*/
				case SDLK_RIGHT:
				{
					if (theGameState == gameState::playing)
						if (theShip.getMapPosition().C < 2)
						{
							theTileMap.update(theShip.getMapPosition(), 1, theShip.getShipRotation());
							theShip.setMapPosition(theShip.getMapPosition().R, theShip.getMapPosition().C + 1);
							theShip.setShipRotation(270);
							theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
						}
				}
				break;
				
				case SDLK_LEFT:
				{
					if (theGameState == gameState::playing)
						if (theShip.getMapPosition().C > 0)
						{
							theTileMap.update(theShip.getMapPosition(), 1, theShip.getShipRotation());
							theShip.setMapPosition(theShip.getMapPosition().R, theShip.getMapPosition().C - 1);
							theShip.setShipRotation(90);
							theTileMap.update(theShip.getMapPosition(), 3, theShip.getShipRotation());
						}
				}
				break;
				case SDLK_SPACE:
				{
					if (theGameState == gameState::playing)
						if (theShip.getMapPosition().C > 0)
						{
							theShip.isStomping = true;
						}
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


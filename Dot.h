#pragma once
#include "LTexture.h"

class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	Dot() {};
	//Initializes the variables
	Dot(bool chaser);
	~Dot();

	void init(SDL_Renderer *gRenderer);
	//Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	//Moves the dot
	void move(int SCREEN_HEIGHT, int SCREEN_WIDTH);

	//Shows the dot on the screen
	void render(SDL_Renderer *gRenderer);

	std::string getPosAsString();

	bool Checkcollision(int centerX, int centerY);

	bool GetLocal() { return isLocalplayer; };

	int GetCenterX();
	int GetCenterY();

	void SetPosition(int x, int y);

private:
	bool isLocalplayer;
	bool isChaser;

	//The X and Y offsets of the dot
	int m_posX, m_posY;

	int m_centerX, m_centerY;
	//The velocity of the dot
	int m_velX, m_velY;

	LTexture m_gDotTexture;
};
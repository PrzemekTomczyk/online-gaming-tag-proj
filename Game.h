#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include "Dot.h"
#include "GameClientHandler.h"
#include "SDL_ttf.h"

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	void processEvents();
	void update();
	void render();
	void cleanup();
	void processGameData();
	void processWinData();
	void gameFinished();
	void restart();
	std::string getErrorString(std::string t_errorMsg);
	// window used in the program
	SDL_Window* m_window;

	// used for drawing things onto the screen
	SDL_Renderer* m_renderer;

	// if game loop is happening
	bool m_isRunning;

	unsigned int m_timeSinceLastSend = 0;
	const bool SEND_DELAY = 50;

	int m_localPosX, m_localPosY;

	Dot m_redDot;
	Dot m_blueDot;
	Dot* m_playerDot;

	GameClientHandler m_gch;

	//consts
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	const int PORT_NUM;
	bool m_isHost;

	int gameStartTime;

	TTF_Font *m_font;

	SDL_Surface* m_textMessage;
	SDL_Texture* m_textTexture;

	bool m_gameOver;
	SDL_Rect m_textRect;
};
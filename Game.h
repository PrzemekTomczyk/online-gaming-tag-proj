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
#include <Windows.h>

class Game
{
	enum class GameState
	{
		Waiting,
		Playing,
		Restarting
	};

public:
	Game();
	~Game();

	void run();

private:
	void processEvents();
	void update();
	void render();
	void cleanup();

	void waitingState();
	void playingState();
	void restartingState();

	void processGameData();
	void processWinData();
	void processStartData();
	void gameFinished();
	void boundaryCheck(float& x, float& y);
	void resetStartPos();
	std::string getErrorString(std::string t_errorMsg);

	// window used in the program
	SDL_Window* m_window;

	// used for drawing things onto the screen
	SDL_Renderer* m_renderer;

	// if game loop is happening
	bool m_isRunning;
	int m_localPosX, m_localPosY;

	//two dots on the screen
	Dot m_hunterDot;//red dot
	Dot m_preyDot;//blue dot
	Dot* m_playerDot;//pointer to either dot that player controls
	Dot* m_otherPlayerDot;//pointer to the other dot the other player controls

	GameClientHandler m_clientHandler;

	//consts
	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	const int PORT_NUM;
	const std::string HOST = "host";
	const std::string GUEST = "guest";
	const std::string GAME_DATA = "gd:";
	const std::string CONNECT_DATA = "cd:";
	const std::string WIN_DATA = "wd:";
	const std::string START_DATA = "sd:";

	bool m_isHost;
	int m_displayTextTime;

	bool m_isColliding;

	TTF_Font* m_font;

	SDL_Surface* m_textMessage;
	SDL_Texture* m_textTexture;

	bool m_gameOver;
	SDL_Rect m_textRect;

	GameState m_state = GameState::Waiting;

	int m_gameTime = 0;

	bool m_hosted = false;
	//Server* m_gameServer;
	STARTUPINFO m_startupInfo = { sizeof(m_startupInfo) };;
	PROCESS_INFORMATION m_processInfo;



	void startGameServer();
	void killGameServer();
};
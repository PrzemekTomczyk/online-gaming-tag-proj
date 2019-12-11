#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include "Dot.h"
#include "GameClientHandler.h"

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
	std::string getErrorString(std::string t_errorMsg);
	// window used in the program
	SDL_Window* m_window;

	// used for drawing things onto the screen
	SDL_Renderer* m_renderer;

	// texture
	SDL_Texture* m_texture;

	// if game loop is happening
	bool isRunning;

	unsigned int m_timeSinceLastSend = 0;
	const bool SEND_DELAY = 50;

	Dot m_playerDot;
	Dot m_otherDot;

	GameClientHandler m_gch;

};
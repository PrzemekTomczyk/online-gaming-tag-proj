#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
#include "Dot.h"

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
	std::string getErrorString(std::string t_errorMsg);
	// window used in the program
	SDL_Window* m_window;

	// used for drawing things onto the screen
	SDL_Renderer* m_renderer;

	// texture
	SDL_Texture* m_texture;

	// if game loop is happening
	bool isRunning;

	Dot m_playerDot;
	Dot m_otherDot;

};
#include "Game.h"

Game::Game()
	:
	m_playerDot{true},
	m_otherDot{false}
{
	try
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw "Error Loading SDL";
		m_window = SDL_CreateWindow("Final Year Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, NULL);
		if (!m_window) throw "Error Loading Window";

		m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		if (!m_renderer) throw "Error Loading Renderer";

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		m_isRunning = true;
		m_playerDot.Init(m_renderer);
		m_otherDot.Init(m_renderer);
	}
	catch (std::string error)
	{
		std::cout << error << std::endl;
		m_isRunning = false;
	}
}

Game::~Game()
{
	cleanup();
}

void Game::run()
{
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;
	Uint32 frameStart;
	int frameTime;
	while (m_isRunning)
	{
		frameStart = SDL_GetTicks();
		frameTime = SDL_GetTicks() - frameStart;
		processEvents();
		update();
		render();
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

void Game::processEvents()
{
	SDL_Event event;
	SDL_PollEvent(&event);

	if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
	{
		//Adjust the velocity
		switch (event.key.keysym.sym)
		{
		case SDLK_ESCAPE: 
			SDL_Quit(); 
			m_isRunning = false;
			break;
		case SDLK_SPACE:
			//m_gch.connectToServer("127.0.0.1", 1111);
			break;
		default:
			break;
		}
	}


	switch (event.type)
	{
	case SDL_QUIT:
		m_isRunning = false;
		break;
	default:
		break;
	}

	m_playerDot.handleEvent(event);
	m_otherDot.handleEvent(event);
}

void Game::update()
{
	//update things here
	m_playerDot.move(600, 800);
	m_otherDot.move(600, 800);

}

void Game::render()
{
	SDL_RenderClear(m_renderer);
	
	//render things here
	m_playerDot.render(m_renderer);
	m_otherDot.render(m_renderer);

	SDL_RenderPresent(m_renderer);
}

void Game::cleanup()
{
	SDL_DestroyTexture(m_texture);
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_QUIT;
}

std::string Game::getErrorString(std::string t_errorMsg)
{
	std::string sdlError = SDL_GetError();
	return t_errorMsg += " " + sdlError;
}

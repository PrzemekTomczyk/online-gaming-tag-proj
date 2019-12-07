#include "Game.h"

Game::Game()
	:
	m_playerDot{true},
	m_otherDot{false},
	WINDOW_WIDTH(800),
	WINDOW_HEIGHT(600),
	PORT_NUM(1111)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		throw getErrorString("Error Loading SDL");
		isRunning = false;
	}
	else
	{
		// create window
		m_window = SDL_CreateWindow("Tag, You're it!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		if (!m_window)
		{
			std::cout << getErrorString("Error Loading Window") << std::endl;
		}
		// create renderer
		m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		if (!m_renderer)
		{
			std::cout << getErrorString("Error Loading Renderer") << std::endl;
		}
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		isRunning = true;

		////load texture
		//m_texture = IMG_LoadTexture(m_renderer, "assets/grid.png");
		//if (!m_texture)
		//{
		//	std::cout << getErrorString("Error Loading Texture") << std::endl;
		//}
		m_playerDot.init(m_renderer);
		m_otherDot.init(m_renderer);
	}
}

Game::~Game()
{
	cleanup();
}

void Game::run()
{
	while (isRunning)
	{
		processEvents();
		update();
		render();
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
			m_gch.disconnect();
			SDL_Quit(); 
			isRunning = false;
			break;
		case SDLK_BACKSPACE:
			m_gch.disconnect();
			break;
		case SDLK_RETURN:
			m_gch.destroyPointer();
			break;
		case SDLK_SPACE:
			// Use below bool to display whether we connected successfully or not or something
			//change local host to user input after adding IP input option
			m_connectedSuccessfully = m_gch.connectToServer(LOCAL_HOST, PORT_NUM);
			break;
		default:
			break;
		}
	}

	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}

	m_playerDot.handleEvent(event);
	m_otherDot.handleEvent(event);
}

void Game::update()
{
	if (m_gch.isConnected())
	{
		//we online bois
		std::string gameData;

		gameData += std::to_string(m_playerDot.GetCenterX());
		gameData += ",";
		gameData += std::to_string(m_playerDot.GetCenterY());

		m_gch.sendGameData(gameData);
	}

	//update things here
	m_playerDot.move(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_otherDot.move(WINDOW_WIDTH, WINDOW_HEIGHT);

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

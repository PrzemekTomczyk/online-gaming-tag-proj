#include "Game.h"


Game::Game()
	:
	m_playerDot{ true },
	m_otherDot{ false }
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

	m_localPosX = m_playerDot.GetCenterX();
	m_localPosY = m_playerDot.GetCenterY();
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
		{
			if (m_gch.isConnected())
			{
				m_gch.disconnect();
			}
			std::cout << "Connect to IP: ";
			std::string ip;
			std::getline(std::cin, ip);
			m_gch.connectToServer(ip, 1111);
			break;
		}
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

	bool collisionDetected = false;
	if (m_isHost)
	{
		if (m_playerDot.collisionDetection(m_otherDot))
		{
			collisionDetected = true;
		}
	}

	if (m_gch.isConnected())
	{
		if (SDL_GetTicks() > m_timeSinceLastSend + SEND_DELAY)
		{
			m_timeSinceLastSend = SDL_GetTicks();
			if (m_localPosX != m_playerDot.GetCenterX() || m_localPosY != m_playerDot.GetCenterY())
			{
				m_gch.sendGameData(m_playerDot.GetCenterX(), m_playerDot.GetCenterY());
				m_localPosX = m_playerDot.GetCenterX();
				m_localPosY = m_playerDot.GetCenterY();
			}
			if (collisionDetected)
			{
				m_gch.sendWinData();
			}
		}
		processGameData();
		processWinData();
	}


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

void Game::processGameData()
{
	if (!m_gch.getGameData().empty())
	{
		std::string& gameData = m_gch.getGameData();
		std::vector<std::string> tokens;
		std::size_t start = 0, end = 0;
		while ((end = gameData.find(",", start)) != std::string::npos) {
			tokens.push_back(gameData.substr(start, end - start));
			start = end + 1;
		}
		tokens.push_back(gameData.substr(start));

		std::vector<int> posVec;

		for (int i = 0; i < tokens.size(); i++)
		{
			std::stringstream ss(tokens.at(i));
			int pos;
			ss >> pos;
			posVec.push_back(pos);
		}

		m_otherDot.SetPosition(posVec[0], posVec[1]);
	}
}

void Game::processWinData()
{
	if (m_gch.getWinData() != "")
	{

	}
}

std::string Game::getErrorString(std::string t_errorMsg)
{
	std::string sdlError = SDL_GetError();
	return t_errorMsg += " " + sdlError;
}

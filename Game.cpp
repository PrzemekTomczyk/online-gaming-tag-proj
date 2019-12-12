#include "Game.h"


Game::Game() :
	m_gameOver{ true },
	m_hunterDot{ true },
	m_preyDot{ false },
	WINDOW_WIDTH(800),
	WINDOW_HEIGHT(600),
	PORT_NUM(1111),
	m_displayTextTime(0)
{
	try
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw "Error Loading SDL";
		if (TTF_Init() < 0) throw "Error Loading SDL TTF";
		m_window = SDL_CreateWindow("Tag, You're it!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, NULL);
		if (!m_window) throw "Error Loading Window";

		m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		if (!m_renderer) throw "Error Loading Renderer";


		m_font = TTF_OpenFont("assets/fonts/comic.ttf", 300);
		if (!m_font) throw "Error Loading Font";

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

		m_isRunning = true;
		m_textRect.x = 0;
		m_textRect.y = 0;

		m_textRect.w = WINDOW_WIDTH;
		m_textRect.h = 150;
		m_hunterDot.init(m_renderer);
		m_preyDot.init(m_renderer);
	}
	catch (std::string error)
	{
		std::cout << error << std::endl;
		m_isRunning = false;
	}
	m_localPosX = -1;
	m_localPosY = -1;

	m_hunterDot.SetPosition(50, 50);
	m_preyDot.SetPosition(750, 550);
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
	SDL_Quit();
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
			m_isRunning = false;
			break;
		case SDLK_SPACE:
		{
			if (m_clientHandler.isConnected())
			{
				//cannot connect to a different server once we connect due to detached thread.
				//m_clientHandler.disconnect();
				std::cout << "Already connected!" << std::endl;
			}
			else
			{
				std::cout << "Connect to IP: ";
				std::string ip;
				std::getline(std::cin, ip);
				m_clientHandler.connectToServer(ip, PORT_NUM);
				while ("" == m_clientHandler.getConnectData())
				{
					//loop until we get data back
				}
				if (HOST == m_clientHandler.getConnectData())
				{
					std::cout << "Host data received" << std::endl;
					m_playerDot = &m_hunterDot;
					m_otherPlayerDot = &m_preyDot;
					m_isHost = true;
				}
				else if (GUEST == m_clientHandler.getConnectData())
				{
					std::cout << "Guest data received" << std::endl;
					m_playerDot = &m_preyDot;
					m_otherPlayerDot = &m_hunterDot;
					m_isHost = false;
					m_state = GameState::Playing;
					m_clientHandler.sendStartData();
				}
			}
			break;
		}
		default:
			break;
		}
	}

	if (m_isRunning)
	{
		if (GameState::Playing == m_state)
		{
			m_playerDot->handleEvent(event);
		}
	}
}

void Game::update()
{
	if (m_isRunning)
	{
		switch (m_state)
		{
		case Game::GameState::Waiting:
			waitingState();
			break;
		case Game::GameState::Playing:
			playingState();
			break;
		case Game::GameState::Restarting:
			restartingState();
			break;
		default:
			break;
		}
	}
}

void Game::render()
{
	if (m_isRunning)
	{
		SDL_RenderClear(m_renderer);

		//render things here
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_textRect);
		m_hunterDot.render(m_renderer);
		m_preyDot.render(m_renderer);

		SDL_RenderPresent(m_renderer);
	}
}

void Game::cleanup()
{
	m_clientHandler.disconnect();

	SDL_DestroyTexture(m_textTexture);
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_QUIT;
}

void Game::waitingState()
{
	if (m_isHost)
	{
		processStartData();
	}
	else
	{
		// second player just chills until host process start game message
	}
}

void Game::playingState()
{
	//update player dot here - does not matter wether host or not
	if (m_playerDot)
	{
		m_playerDot->move(WINDOW_WIDTH, WINDOW_HEIGHT);
		if (m_localPosX != m_playerDot->GetCenterX() || m_localPosY != m_playerDot->GetCenterY())
		{
			m_clientHandler.sendGameData(m_playerDot->GetCenterX(), m_playerDot->GetCenterY());
			m_localPosX = m_playerDot->GetCenterX();
			m_localPosY = m_playerDot->GetCenterY();
		}
	}
	//process received data from other player
	processGameData();

	//host/client specific processing
	if (m_isHost)
	{
		//check if colliding with other player
		if (m_playerDot->collisionDetection(*m_otherPlayerDot))
		{
			m_state = GameState::Restarting;
			m_gameTime = SDL_GetTicks() - m_gameTime;
			m_clientHandler.sendWinData(m_gameTime);
			return;
		}
	}
	else
	{
		processWinData();
	}
}

void Game::restartingState()
{
	if (m_isHost)
	{
		processWinData();
	}
	else
	{
		m_state = GameState::Playing;
		resetStartPos();
		m_clientHandler.sendStartData();
	}
}

void Game::processGameData()
{
	if (!m_clientHandler.getGameData().empty())
	{
		std::string& gameData = m_clientHandler.getGameData();
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

		m_otherPlayerDot->SetPosition(posVec[0], posVec[1]);
	}
}

void Game::processWinData()
{
	if (!m_clientHandler.getWinData().empty())
	{
		std::string& winData = m_clientHandler.getWinData();
		gameFinished();
		winData = "";
		m_state = GameState::Restarting;
	}
	else if (m_isHost)
	{
		gameFinished();
		resetStartPos();
		m_state = GameState::Waiting;
	}
}

void Game::processStartData()
{
	if (!m_clientHandler.getStartData().empty())
	{
		std::cout << "Starting game" << std::endl;
		m_gameTime = SDL_GetTicks();
		m_state = GameState::Playing;
	}
}

void Game::gameFinished()
{
	std::string gameOverText = "";
	if (m_isHost)
	{
		gameOverText = "Collision Detected Time Elapsed:" + std::to_string(m_gameTime) + " Milliseconds";
	}
	else
	{
		gameOverText = "Collision Detected Time Elapsed:" + m_clientHandler.getWinData() + " Milliseconds";
	}

	m_textMessage = TTF_RenderText_Blended(m_font, gameOverText.c_str(), SDL_Color{ 255,255,0,255 });
	if (!m_textMessage) std::cout << "Error Loading Surface" << std::endl;
	m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_textMessage);
	if (!m_textTexture) std::cout << "Error Loading Texture" << std::endl;
}

void Game::boundaryCheck(float& x, float& y)
{
	if (x < 0)
	{
		x = 20;
	}
	else if (x > WINDOW_WIDTH)
	{
		x = WINDOW_WIDTH - 20;
	}
	if (y < 0)
	{
		y = 20;
	}
	else if (y > WINDOW_HEIGHT)
	{
		y = WINDOW_HEIGHT - 20;
	}
}

void Game::resetStartPos()
{
	m_hunterDot.SetPosition(50, 50);
	m_hunterDot.resetVelo();
	m_preyDot.SetPosition(750, 550);
	m_preyDot.resetVelo();

	if (m_playerDot == &m_hunterDot)
	{
		m_playerDot = &m_preyDot;
		m_otherPlayerDot = &m_hunterDot;
	}
	else
	{
		m_playerDot = &m_hunterDot;
		m_otherPlayerDot = &m_preyDot;
	}
}

std::string Game::getErrorString(std::string t_errorMsg)
{
	std::string sdlError = SDL_GetError();
	return t_errorMsg += " " + sdlError;
}

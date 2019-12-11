#include "Game.h"


Game::Game()
	:
	m_playerDot{ true },
	m_otherDot{ false },
	m_gameOver{false}
{
	try
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw "Error Loading SDL";
		if (TTF_Init() < 0) throw "Error Loading SDL TTF";
		m_window = SDL_CreateWindow("Final Year Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, NULL);
		if (!m_window) throw "Error Loading Window";

		m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		if (!m_renderer) throw "Error Loading Renderer";


		m_font = TTF_OpenFont("assets/fonts/comic.ttf", 300);
		if (!m_font) throw "Error Loading Font";

		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		m_isRunning = true;
		m_playerDot.Init(m_renderer);
		m_otherDot.Init(m_renderer);
		gameStartTime = SDL_GetTicks();
		m_textRect.x = 0;
		m_textRect.y = 0;
		m_textRect.w = 600;
		m_textRect.h = 250;
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
		case SDLK_r:
		{
			if (m_gameOver)
			{
				restart();
			}
			break;
		}
		case SDLK_g:
		{
			if (!m_gameOver)
			{
				gameFinished();
			}
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
	if (!m_gameOver)
	{
		m_playerDot.handleEvent(event);
		m_otherDot.handleEvent(event);
	}
}

void Game::update()
{
	//update things here
	if (!m_gameOver)
	{
		m_playerDot.move(600, 800);
		m_otherDot.move(600, 800);

		bool collisionDetected = false;

		if (m_playerDot.collisionDetection(m_otherDot))
		{
			collisionDetected = true;
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


}

void Game::render()
{
	SDL_RenderClear(m_renderer);

	//render things here
	m_playerDot.render(m_renderer);
	m_otherDot.render(m_renderer);
	if (m_gameOver)
	{
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_textRect);
	}

	SDL_RenderPresent(m_renderer);
}

void Game::cleanup()
{
	SDL_DestroyTexture(m_texture);
	SDL_DestroyTexture(m_textTexture);
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
	if (!m_gch.getWinData().empty())
	{
		std::string& winData = m_gch.getWinData();
		if (winData == "Win")
		{
			gameFinished();
			std::cout << "OUCH!" << std::endl;
		}
	}
}

void Game::gameFinished()
{
	m_gameOver = true;
	int gameTime = SDL_GetTicks() - gameStartTime;
	std::string gameOverText = "Game Won\nTime Elapsed:\n" + std::to_string(gameTime) + " Milliseconds\nPress R To Restart";
	m_textMessage = TTF_RenderText_Blended_Wrapped(m_font, gameOverText.c_str(), SDL_Color{ 255,255,0,255 }, 5000);
	if (!m_textMessage) std::cout << "Error Loading Surface" << std::endl;
	m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_textMessage);
	if (!m_textTexture) std::cout << "Error Loading Texture" << std::endl;
}

void Game::restart()
{
	m_gameOver = false;
	gameStartTime = SDL_GetTicks();
	m_playerDot.SetPosition(50,50);
	m_otherDot.SetPosition(750, 550);
}

std::string Game::getErrorString(std::string t_errorMsg)
{
	std::string sdlError = SDL_GetError();
	return t_errorMsg += " " + sdlError;
}

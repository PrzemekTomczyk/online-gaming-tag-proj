#include "Game.h"


Game::Game() :
	m_gameOver{ true },
	m_hunterDot{ true },
	m_preyDot{ false },
	m_gameStarted{ false },
	WINDOW_WIDTH(800),
	WINDOW_HEIGHT(600),
	PORT_NUM(1111)
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
		gameStartTime = SDL_GetTicks();
		m_textRect.x = 0;
		m_textRect.y = 0;
		m_textRect.w = 600;
		m_textRect.h = 250;
		m_hunterDot.init(m_renderer);
		m_preyDot.init(m_renderer);
	}
	catch (std::string error)
	{
		std::cout << error << std::endl;
		m_isRunning = false;
	}

	m_localPosX = -1;//m_hunterDot.GetCenterX();
	m_localPosY = -1;//m_hunterDot.GetCenterY();

	m_hunterDot.SetPosition(50, 50);
	m_preyDot.SetPosition(750, 550);


	//#############################################################
	//temporarily hardcode redDot to be the player!
	//m_playerDot = &m_hunterDot;
	//#############################################################
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
		case SDLK_BACKSPACE:
			m_gch.disconnect();
			break;
		case SDLK_RETURN:
			m_gch.connectToServer("127.0.0.1", PORT_NUM); 
			m_playerDot = &m_hunterDot;
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
			m_gch.connectToServer(ip, PORT_NUM);
			while ("" == m_gch.getConnectData())
			{
				//loop until we get data back
			}
			if (HOST == m_gch.getConnectData())
			{
				m_playerDot = &m_hunterDot;
				m_otherPlayerDot = &m_preyDot;
				m_isHost = true;
			}
			else if (!m_isHost)
			{
				m_playerDot = &m_preyDot;
				m_otherPlayerDot = &m_hunterDot;
				m_isHost = false;
				m_gameOver = false;
			}
			else
			{
				restart();
			}
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
	if (!m_gameOver || m_playerDot)
	{
		m_playerDot->handleEvent(event);
		//m_preyDot.handleEvent(event);
	}
}

void Game::update()
{
	//update things here
	if (!m_gameOver)
	{
		if (m_playerDot)
		{
			m_playerDot->move(WINDOW_WIDTH, WINDOW_HEIGHT);
		}

		if (m_gch.isConnected())
		{
			//we online bois

			bool collisionDetected = false;
			if (m_isHost)
			{
				if (m_playerDot->collisionDetection(*m_otherPlayerDot))
				{
					collisionDetected = true;
				}
			}

			if (SDL_GetTicks() > m_timeSinceLastSend + SEND_DELAY)
				m_timeSinceLastSend = SDL_GetTicks();

			if (m_localPosX != m_playerDot->GetCenterX() || m_localPosY != m_playerDot->GetCenterY())
			{
				m_gch.sendGameData(m_playerDot->GetCenterX(), m_playerDot->GetCenterY());
				m_localPosX = m_playerDot->GetCenterX();
				m_localPosY = m_playerDot->GetCenterY();
			}
			if (collisionDetected && m_isHost)
			{
				m_gch.sendWinData();
				processGameData();
				processWinData();
			}
		}
	}
	else if (HOST != m_gch.getConnectData() && m_isHost && !m_gameStarted)
	{
		restart();
		m_gameStarted = true;
	}
	else if (m_playerDot)
	{
		m_playerDot->move(WINDOW_WIDTH, WINDOW_HEIGHT);
		if (m_localPosX != m_playerDot->GetCenterX() || m_localPosY != m_playerDot->GetCenterY())
		{
			m_gch.sendGameData(m_playerDot->GetCenterX(), m_playerDot->GetCenterY());
			m_localPosX = m_playerDot->GetCenterX();
			m_localPosY = m_playerDot->GetCenterY();
		}
	}
}

void Game::render()
{
	SDL_RenderClear(m_renderer);

	//render things here
	if (m_gameOver)
	{
		SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_textRect);
	}
	m_hunterDot.render(m_renderer);
	m_preyDot.render(m_renderer);

	SDL_RenderPresent(m_renderer);
}

void Game::cleanup()
{
	m_gch.disconnect();

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

		m_otherPlayerDot->SetPosition(posVec[0], posVec[1]);
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
	if (m_isHost)
	{
		m_playerDot->SetPosition(50, 50);
	}
	else
	{
		m_playerDot->SetPosition(750, 550);
	}
}

std::string Game::getErrorString(std::string t_errorMsg)
{
	std::string sdlError = SDL_GetError();
	return t_errorMsg += " " + sdlError;
}

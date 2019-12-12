#include "Game.h"


Game::Game()
	:
	m_redDot{ true },
	m_blueDot{ false },
	WINDOW_WIDTH(800),
	WINDOW_HEIGHT(600),
	PORT_NUM(1111),
	displayTextTime(0)
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
		m_textRect.w = 400;
		m_textRect.h = 150;
		m_redDot.init(m_renderer);
		m_blueDot.init(m_renderer);
	}
	catch (std::string error)
	{
		std::cout << error << std::endl;
		m_isRunning = false;
	}

	m_localPosX = m_redDot.GetCenterX();
	m_localPosY = m_redDot.GetCenterY();

	//#############################################################
	//temporarily hardcode redDot to be the player!
	m_playerDot = &m_redDot;
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
			m_isRunning = false;
			break;
		case SDLK_BACKSPACE:
			m_gch.disconnect();
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
			break;
		}
		case SDLK_r:
		{
			restart();
			break;
		}
		case SDLK_g:
		{
			gameFinished();
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
	m_redDot.handleEvent(event);
	m_blueDot.handleEvent(event);
}

void Game::update()
{
	//update things here
	//m_blueDot.move(WINDOW_WIDTH, WINDOW_HEIGHT);

	//m_redDot.move(WINDOW_WIDTH, WINDOW_HEIGHT);
	//m_blueDot.move(WINDOW_WIDTH, WINDOW_HEIGHT);

	if (m_playerDot)
	{
		m_playerDot->move(WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	bool collisionDetected = false;
	if (m_redDot.collisionDetection(m_blueDot))
	{
		collisionDetected = true;
	}

	if (m_gch.isConnected())
	{
		//we online bois
		if (SDL_GetTicks() > m_timeSinceLastSend + SEND_DELAY)
		m_timeSinceLastSend = SDL_GetTicks();
		if (m_localPosX != m_redDot.GetCenterX() || m_localPosY != m_redDot.GetCenterY())
		{
			m_gch.sendGameData(m_redDot.GetCenterX(), m_redDot.GetCenterY());
			m_localPosX = m_redDot.GetCenterX();
			m_localPosY = m_redDot.GetCenterY();
		}
		if (SDL_GetTicks() > m_timeSinceLastSend + SEND_DELAY)
		{
			m_timeSinceLastSend = SDL_GetTicks();
			if (m_localPosX != m_playerDot->GetCenterX() || m_localPosY != m_playerDot->GetCenterY())
			{
				m_gch.sendGameData(m_playerDot->GetCenterX(), m_playerDot->GetCenterY());
				m_localPosX = m_playerDot->GetCenterX();
				m_localPosY = m_playerDot->GetCenterY();
			}

		}
		if (collisionDetected)
		{
			m_gch.sendWinData();
			processWinData();
		}
		else
		{
			m_isColliding = false;
		}
		processGameData();
		processWinData();
			
	}
}

void Game::render()
{
	SDL_RenderClear(m_renderer);

	//render things here

	SDL_RenderCopy(m_renderer, m_textTexture, NULL, &m_textRect);
	m_redDot.render(m_renderer);
	m_blueDot.render(m_renderer);

	SDL_RenderPresent(m_renderer);
}

void Game::cleanup()
{
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

		m_blueDot.SetPosition(posVec[0], posVec[1]);
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
			winData = "";
			m_isColliding = false;
		}
	}
}

void Game::gameFinished()
{
	if (!m_isColliding)
	{
		int gameTime = SDL_GetTicks() - gameStartTime;
		std::string gameOverText = "Collision Detected Time Elapsed:" + std::to_string(gameTime) + " Milliseconds";
		m_textMessage = TTF_RenderText_Blended(m_font, gameOverText.c_str(), SDL_Color{ 255,255,0,255 });
		if (!m_textMessage) std::cout << "Error Loading Surface" << std::endl;
		m_textTexture = SDL_CreateTextureFromSurface(m_renderer, m_textMessage);
		if (!m_textTexture) std::cout << "Error Loading Texture" << std::endl;
		moveDot();
		gameStartTime = SDL_GetTicks();
		m_isColliding = true;
	}
}

void Game::restart()
{
	gameStartTime = SDL_GetTicks();
	if (m_isHost)
	{
		m_redDot.SetPosition(50, 50);

	}
	else
	{
		m_redDot.SetPosition(750, 550);

	}

}

void Game::moveDot()
{
	//#include <random>
	//#include <iostream>

	//	int main()
	//	{
	//		std::random_device dev;
	//		std::mt19937 rng(dev());
	//		std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 6); // distribution in range [1, 6]

	//		std::cout << dist6(rng) << std::endl;
	//	}
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 360);
	//pI/180
	float xMovement, yMovement;
	//hsinanfle = y
	xMovement = (100) * std::cos((dist(rng) * (M_PI / 180)));
	yMovement = (100) * std::sin((dist(rng) * (M_PI / 180)));
	xMovement += m_redDot.GetCenterX();
	yMovement += m_redDot.GetCenterY();
	boundaryCheck(xMovement, yMovement);
	m_redDot.SetPosition(xMovement, yMovement);
}

void Game::boundaryCheck(float& x, float& y)
{
	if (x < 0)
	{
		x = 20;
	}
	else if (x > WINDOW_WIDTH)
	{
		x = WINDOW_WIDTH -20;
	}
	if (y < 0)
	{
		y = 20;
	}
	else if (y > WINDOW_HEIGHT)
	{
		y = WINDOW_HEIGHT -20;
	}
}

std::string Game::getErrorString(std::string t_errorMsg)
{
	std::string sdlError = SDL_GetError();
	return t_errorMsg += " " + sdlError;
}

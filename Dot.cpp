#include "Dot.h"

Dot::Dot(bool chaser)
{
	m_isChaser = chaser;
	//Initialize the offsets
	m_posX = 0;
	m_posY = 0;

	//Initialize the velocity
	m_velX = 0;
	m_velY = 0;

	if (m_isChaser)
	{
		m_dotTexture.setColor(244, 66, 66);
	}
	else
	{
		m_dotTexture.setColor(66, 69, 244);
	}
}

Dot::~Dot()
{
	m_dotTexture.free();
}

void Dot::SetPosition(int x, int y)
{
	m_centerX = x;
	m_centerY = y;
	
	m_posX = m_centerX - (DOT_WIDTH / 2);
	m_posY = m_centerY - (DOT_HEIGHT / 2);
}

//"dot.bmp"
void Dot::Init(SDL_Renderer* gRenderer)
{
	if (m_isChaser)
	{
		if (!m_dotTexture.loadFromFile("assets/reddot.bmp", gRenderer))
		{
			printf("Failed to load dot texture!\n");
		}
	}
	else
	{
		if (!m_dotTexture.loadFromFile("assets/bluedot.bmp", gRenderer))
		{
			printf("Failed to load dot texture!\n");
		}
	}
}

void Dot::handleEvent(SDL_Event& e)
{
	if (m_isChaser)
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_w: m_velY -= DOT_VEL; break;
			case SDLK_s: m_velY += DOT_VEL; break;
			case SDLK_a: m_velX -= DOT_VEL; break;
			case SDLK_d: m_velX += DOT_VEL; break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_w: m_velY += DOT_VEL; break;
			case SDLK_s: m_velY -= DOT_VEL; break;
			case SDLK_a: m_velX += DOT_VEL; break;
			case SDLK_d: m_velX -= DOT_VEL; break;
			}
		}
	}
	else if (!m_isChaser)
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: m_velY -= DOT_VEL; break;
			case SDLK_DOWN: m_velY += DOT_VEL; break;
			case SDLK_LEFT: m_velX -= DOT_VEL; break;
			case SDLK_RIGHT: m_velX += DOT_VEL; break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: m_velY += DOT_VEL; break;
			case SDLK_DOWN: m_velY -= DOT_VEL; break;
			case SDLK_LEFT: m_velX += DOT_VEL; break;
			case SDLK_RIGHT: m_velX -= DOT_VEL; break;
			}
		}
	}
}

void Dot::move(int SCREEN_HEIGHT, int SCREEN_WIDTH)
{
	//Move the dot left or right
	m_posX += m_velX;

	//If the dot went too far to the left or right
	if ((m_posX < 0) || (m_posX + DOT_WIDTH > SCREEN_WIDTH))
	{
		//Move back
		m_posX -= m_velX;
	}

	//Move the dot up or down
	m_posY += m_velY;

	//If the dot went too far up or down
	if ((m_posY < 0) || (m_posY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		m_posY -= m_velY;
	}

	m_centerX = m_posX + (DOT_WIDTH / 2);
	m_centerY = m_posY + (DOT_HEIGHT / 2);
}

void Dot::render(SDL_Renderer* gRenderer)
{
	//Show the dot
	m_dotTexture.render(m_posX, m_posY, gRenderer);
}

std::string Dot::GetPosAsString()
{
	return std::string("X: " + std::to_string(m_posX) + ", " + "Y: " + std::to_string(m_posY));
}

int Dot::GetCenterX()
{
	return m_centerX;
}

int Dot::GetCenterY()
{
	return m_centerY;
}

bool Dot::Checkcollision(int centerX, int centerY)
{
	int distance = sqrt(((m_centerX - centerX) * (m_centerX - centerX)) + ((m_centerY - centerY) * (m_centerY - centerY)));

	return distance <= DOT_WIDTH;
}
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

bool Dot::collisionDetection(Dot& t_otherDot)
{
	float distanceBetween = sqrt(std::pow(t_otherDot.m_centerX - m_centerX, 2.0f) + std::pow(t_otherDot.m_centerY - m_centerY, 2.0f));
	return distanceBetween <= DOT_HEIGHT;
}

//"dot.bmp"
void Dot::init(SDL_Renderer* gRenderer)
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
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_w: m_w = true; break;
		case SDLK_s: m_s = true; break;
		case SDLK_a: m_a = true; break;
		case SDLK_d: m_d = true; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_w: m_w = false; break;
		case SDLK_s: m_s = false; break;
		case SDLK_a: m_a = false; break;
		case SDLK_d: m_d = false; break;
		}
	}
}

void Dot::move(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	if (m_w) 
	{
		m_posY -= DOT_VEL;
	}
	if (m_s)
	{
		m_posY += DOT_VEL;
	}
	if (m_a)
	{
		m_posX -= DOT_VEL;
	}
	else if (m_d)
	{
		m_posX += DOT_VEL;
	}

	//Move the dot left or right
	//m_posX += m_velX;

	//If the dot went too far to the left or right
	if (m_posX < 0)
	{
		//Move back
		m_posX = 0;
	}
	else if (m_posX + DOT_WIDTH > SCREEN_WIDTH)
	{
		m_posX = SCREEN_WIDTH - DOT_WIDTH;
	}

	//Move the dot up or down
	//m_posY += m_velY;

	//If the dot went too far up or down
	if (m_posY < 0)
	{
		//Move back
		m_posY = 0;
	}
	else if (m_posY + DOT_HEIGHT > SCREEN_HEIGHT)
	{
		m_posY = SCREEN_HEIGHT - DOT_HEIGHT;
	}

	m_centerX = m_posX + (DOT_WIDTH / 2);
	m_centerY = m_posY + (DOT_HEIGHT / 2);
}

void Dot::resetVelo()
{
	m_w = false;
	m_s = false;
	m_a = false;
	m_d = false;

	m_velX = 0;
	m_velY = 0;
}

void Dot::render(SDL_Renderer* gRenderer)
{
	//Show the dot
	m_dotTexture.render(m_posX, m_posY, gRenderer);
}

std::string Dot::getPosAsString()
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
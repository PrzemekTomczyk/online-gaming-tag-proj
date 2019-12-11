#include "GameClientHandler.h"

GameClientHandler::GameClientHandler() :
	m_client(nullptr)
{
}

GameClientHandler::~GameClientHandler()
{
	delete m_client;
}

void GameClientHandler::connectToServer(std::string t_ip, int t_port)
{
	if (m_client)
	{
		disconnect();
	}

	m_client = new Client(t_ip.c_str(), t_port, m_gameData);

	if (!m_client->Connect())
	{
		std::cout << "Failed to connect to server..." << std::endl;
	}
}

void GameClientHandler::disconnect()
{
	m_client->Disconnect();
	delete m_client;
	m_client = nullptr;
}

std::string& GameClientHandler::getGameData()
{
	return m_gameData;
}

bool GameClientHandler::isConnected()
{
	if (m_client)
	{
		return m_client->getConnected();
	}
	else
	{
		return false;
	}
}

void GameClientHandler::sendGameData(int t_posX, int t_posY)
{
	std::string data = "";
	data += std::to_string(t_posX);
	data += ",";
	data += std::to_string(t_posY);
	m_client->SendGameData(data);
}

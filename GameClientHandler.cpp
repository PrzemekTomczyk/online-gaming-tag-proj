#include "GameClientHandler.h"

GameClientHandler::GameClientHandler() :
	m_client(nullptr)
{
}

GameClientHandler::~GameClientHandler()
{
	if (m_client)
	{
		if (m_client->getConnected())
		{
			m_client->Disconnect();
		}
		delete m_client;
	}
	m_client = nullptr;
}

void GameClientHandler::connectToServer(std::string t_ip, int t_port)
{
	if (m_client)
	{
		disconnect();
	}

	if (t_ip.empty())
	{
		m_client = new Client(LOCAL_HOST.c_str(), t_port, m_gameData, m_winData, m_connectData, m_gameStartData);
	}
	else
	{
		m_client = new Client(t_ip.c_str(), t_port, m_gameData, m_winData, m_connectData, m_gameStartData);
	}

	if (!m_client->Connect())
	{
		std::cout << "Failed to connect to server..." << std::endl;
	}
	sendConnectData();
}


void GameClientHandler::disconnect()
{
	if (m_client)
	{
		if (m_client->getConnected())
		{
			m_client->Disconnect();
		}
		delete m_client;
	}
	m_client = nullptr;
}

std::string& GameClientHandler::getGameData()
{
	return m_gameData;
}

std::string& GameClientHandler::getWinData()
{
	return m_winData;
}

bool GameClientHandler::getGameStart()
{
	return m_gameStartData == "start";
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

void GameClientHandler::sendWinData()
{
	std::string data = "Win";
	m_client->SendWinData(data);
}

void GameClientHandler::sendConnectData()
{
	std::string data = "";
	m_client->SendConnectData(data);
}

std::string GameClientHandler::getConnectData()
{
	return m_connectData;
}
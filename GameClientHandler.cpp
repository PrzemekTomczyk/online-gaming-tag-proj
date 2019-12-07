#include "GameClientHandler.h"

GameClientHandler::GameClientHandler() :
	m_client(nullptr)
{
}

GameClientHandler::~GameClientHandler()
{
	delete m_client;
}

bool GameClientHandler::connectToServer(std::string t_ip, int t_port)
{
	if (m_client)
	{
		delete m_client;
		m_client = nullptr;
	}

	m_client = new Client(t_ip.c_str(), t_port);

	if (!m_client->Connect())
	{
		m_isConnected = false;
		return false;
	}

	m_isConnected = true;
	return true;
}

void GameClientHandler::disconnect()
{
	m_client->Disconnect();
	//delete m_client;
	//m_client = nullptr;
}

void GameClientHandler::sendGameData(std::string t_gameData)
{
	m_client->SendGameData(t_gameData);
}

void GameClientHandler::destroyPointer()
{
	delete m_client;
	m_client = nullptr;
}

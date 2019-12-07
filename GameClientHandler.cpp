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
		std::cout << "Failed to connect to server..." << std::endl;
		return false;
	}

	return true;
}

bool GameClientHandler::disconnect()
{
	try
	{
		m_client->Disconnect();
		delete m_client;
		m_client = nullptr;
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

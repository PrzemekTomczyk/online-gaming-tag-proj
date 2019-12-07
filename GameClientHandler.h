#include "Client.h"
#include <iostream>

class GameClientHandler
{
public:
	GameClientHandler();
	~GameClientHandler();

	bool connectToServer(std::string t_ip, int t_port);
	void disconnect();
	bool isConnected() { return m_isConnected; }
	void sendGameData(std::string t_gameData);
	void destroyPointer();

private:
	Client* m_client;
	bool m_isConnected = false;
};
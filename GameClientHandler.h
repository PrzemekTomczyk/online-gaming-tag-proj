#include "Client.h"
#include <iostream>
#include <string>

class GameClientHandler
{
public:
	GameClientHandler();
	~GameClientHandler();

	void connectToServer(std::string t_ip, int t_port);
	void disconnect();
	std::string& getGameData();
	bool isConnected();
	void sendGameData(int t_posX, int t_posY);

private:
	std::string m_gameData;
	Client* m_client;
};
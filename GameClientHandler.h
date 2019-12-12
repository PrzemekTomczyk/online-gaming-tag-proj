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
	std::string& getWinData();
	bool getGameStart();
	bool isConnected();
	void sendGameData(int t_posX, int t_posY);
	void sendWinData();
	void sendConnectData();
	std::string getConnectData();

private:
	std::string m_gameData;
	std::string m_winData;
	std::string m_connectData;
	std::string m_gameStartData;
	Client* m_client;

	const std::string LOCAL_HOST = "127.0.0.1";
};
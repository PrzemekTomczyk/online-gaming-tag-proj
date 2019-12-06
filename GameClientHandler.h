#include "Client.h"
#include <iostream>

class GameClientHandler
{
public:
	GameClientHandler();
	~GameClientHandler();

	bool connectToServer(std::string t_ip, int t_port);
	bool disconnect();

private:
	Client* m_client;
};
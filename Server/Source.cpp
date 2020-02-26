#include "Server.h"
#include <chrono>
#include <iostream>

int main()
{
	Server MyServer(1111, false); //Create server on port 1111, false=do not loopback to localhost (others can connect)
	auto t1 = std::chrono::high_resolution_clock::now();
	float startTime = clock();
	float endTime = clock();
	bool running = true;
	while (running)
	{
		if (MyServer.getConnectionCount() == 0 && (endTime - startTime) / (double)CLOCKS_PER_SEC <= 30)
		{
			MyServer.ListenForNewConnection(); //Accept new connection (if someones trying to connect)
			endTime = clock();
		}
		else if (MyServer.getConnectionCount() == 0 && (endTime - startTime) / (double)CLOCKS_PER_SEC > 30)
		{
			running = false;
		}
		else if (MyServer.getConnectionCount() > 0)
		{
			MyServer.ListenForNewConnection(); //Accept new connection (if someones trying to connect)
			startTime = clock();
			endTime = clock();
		}
		std::cout << ((endTime - startTime) / (double)CLOCKS_PER_SEC) << std::endl;
	}
	std::cout << "Server timed out" << std::endl;
	return 0;
}
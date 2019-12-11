#include "Server.h"
#include <iostream>
#include "PacketStructs.h"
#pragma comment(lib,"ws2_32.lib") //Required for WinSock

bool Server::ProcessPacket(std::shared_ptr<Connection> connection, PacketType packetType)
{
	switch (packetType)
	{
	case PacketType::ChatMessage: //Packet Type: chat message
	{
		std::string message; //string to store our message we received
		if (!GetString(connection, message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false

		//Next we need to send the message out to each user
		PS::ChatMessage cm(message);
		std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(cm.toPacket()); //use shared_ptr instead of sending with SendString so we don't have to reallocate packet for each connection
		{
			std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);
			for (auto conn : m_connections) //For each connection...
			{
				if (conn == connection) //If connection is the user who sent the message...
					continue;//Skip to the next user since there is no purpose in sending the message back to the user who sent it.
				conn->m_pm.Append(msgPacket);
			}
		}
		std::cout << "Processed chat message packet from user ID: " << connection->m_ID << std::endl;
		break;
	}
	case PacketType::GameData: //Packet Type: gameData
	{
		std::string gameData; //string to store our gameData we received
		if (!GetString(connection, gameData)) //Get the gameData and store it in variable: gameData
			return false; //If we do not properly get the gameData, return false
						  //Next we need to send the gameData out to each user

		PS::GameData gd(gameData);
		std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(gd.toPacket()); //use shared_ptr instead of sending with SendString so we don't have to reallocate packet for each connection
		{
			std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);
			for (auto conn : m_connections) //For each connection...
			{
				if (conn == connection) //If connection is the user who sent the gameData...
					continue;//Skip to the next user since there is no purpose in sending the gameData back to the user who sent it.
				conn->m_pm.Append(msgPacket);
			}
		}
		std::cout << "Processed gameData packet from user ID: " << connection->m_ID << std::endl;
		break;
	}
	case PacketType::WinData:
	{
		std::string winData;
		if (!GetString(connection, winData))
			return false;
		PS::WinData wd(winData);
		std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(wd.toPacket());
		{
			std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);
			for (auto conn : m_connections)
			{
				if (conn == connection)
					continue;
				conn->m_pm.Append(msgPacket);
			}
		}
		std::cout << "Processed Win Data packet from user ID: " << connection->m_ID << std::endl;
		break;
	}
	case PacketType::ConnectionData: //Packet Type: gameData
	{
		std::string connectData; //string to store our connectData we received
		//if (!GetString(connection, connectData)) //Get the connectData and store it in variable: connectData
		//	return false; //If we do not properly get the connectData, return false
						  //Next we need to send the connectData out to each user

		{
			std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);

			//if this packet is received and we only ahve 1 user
			//then this user should be host
			if (1 == m_connections.size())
			{
				connectData = "host";
			}
			else
			{
				connectData = "guest";
			}

			PS::ConnectionData cd(connectData);
			std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(cd.toPacket()); //use shared_ptr instead of sending with SendString so we don't have to reallocate packet for each connection
			connection->m_pm.Append(msgPacket);

		}
		std::cout << "Processed connectData packet from user ID: " << connection->m_ID << std::endl;
		break;
	}
	case PacketType::FileTransferRequestFile:
	{
		std::string fileName; //string to store file name
		if (!GetString(connection, fileName)) //If issue getting file name
			return false; //Failure to process packet
		std::string errMsg;
		if (connection->m_file.Initialize(fileName, errMsg)) //if filetransferdata successfully initialized
		{
			connection->m_pm.Append(connection->m_file.getOutgoingBufferPacket()); //Send first buffer from file
		}
		else //If initialization failed, send error message
		{
			SendString(connection, errMsg);
		}
		break;
	}
	case PacketType::FileTransferRequestNextBuffer:
	{
		if (connection->m_file.m_transferInProgress)
		{
			connection->m_pm.Append(connection->m_file.getOutgoingBufferPacket()); //get and send next buffer for file
		}
		break;
	}
	default: //If packet type is not accounted for
	{
		std::cout << "Unrecognized packet: " << (std::int32_t)packetType << std::endl; //Display that packet was not found
		return false;
	}
	}
	return true;
}
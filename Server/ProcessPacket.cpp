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
		std::string message = "other:gd:10,20"; //string to store our message we received
		if (!GetString(connection, message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
		else
		{
			//break first part of string off
			if (message.find("all:") != std::string::npos)
			{
				std::string data = message.erase(message.find("all:"), 4);
				//send to all connections, including self
				//Next we need to send the message out to each user
				PS::ChatMessage cm(message);
				std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(cm.toPacket()); //use shared_ptr instead of sending with SendString so we don't have to reallocate packet for each connection
				{
					std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);
					for (auto conn : m_connections) //For each connection...
					{
						conn->m_pm.Append(msgPacket);
					}
				}
			}
			else if (message.find("other:") != std::string::npos)
			{
				std::string data = message.erase(message.find("other:"), 6);

				//dont sent to self
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
			}
			else if (message.find("self:") != std::string::npos) //this should be only ConnectData packetType
			{
				std::string data = message.erase(message.find("self:"), 5);

				if (1 == m_connections.size())
				{
					data += "host";
				}
				else
				{
					data += "guest";
				}

				//dont sent to self
				PS::ChatMessage cm(data);
				std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(cm.toPacket()); //use shared_ptr instead of sending with SendString so we don't have to reallocate packet for each connection
				{
					std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);

					connection->m_pm.Append(msgPacket);
				}
			}
		}

		//std::cout << "Processed packet from user ID: " << connection->m_ID << std::endl;
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
		std::cout << "Unrecognized packet: " << (std::int32_t)packetType << " from user: " << connection->m_ID << std::endl; //Display that packet was not found
		std::string unknownPacket;
		if (!GetString(connection, unknownPacket))
			std::cout << unknownPacket << std::endl;

		return false;
	}
	}
	return true;
}
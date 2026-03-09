#include "ServerManager.h"
#include <iostream>
#include <thread>

ServerManager::ServerManager()
{
	WSADATA wsaData;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
}

ServerManager::~ServerManager()
{
	closesocket(serverSocket);
	WSACleanup();
}

bool ServerManager::StartServer(int port)
{
	if (!db.ConnectDB("localhost", "root", "1234", "GomokuDB", 3306))
	{
		std::cout << "[서버] DB 연결 실패" << "\n";
		return false;
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSocket == INVALID_SOCKET)
	{
		return false;
	}

	SOCKADDR_IN serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return false;
	}

	std::cout << "[서버] DB 연결 성공, " << port << "번 포트에서 접속 대기 중..." << "\n";

	return true;
}

void ServerManager::AcceptClients()
{
	while (true)
	{
		sockaddr_in  clientAddr;
		int addrLen = sizeof(clientAddr);

		SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen);

		if (clientSocket != INVALID_SOCKET)
		{
			GamePacket packet;

			if (recv(clientSocket, (char*)&packet, sizeof(GamePacket), 0) > 0)
			{
				if (packet.type == LOGIN)
				{
					bool isValid = db.Login(packet.id, packet.pw);

					GamePacket response = {};
					response.type = LOGIN;

					if (isValid)
					{
						std::cout << "[서버] 로그인 승인: " << packet.id << "\n";
						response.x = 1;
						send(clientSocket, (char*)&response, sizeof(GamePacket), 0);

						std::cout << "[서버] 새로운 유저가 접속했습니다." << "\n";

						connClients.push_back(std::make_pair(clientSocket, response.id));
					}
					else
					{
						std::cout << "[서버] 로그인 거부: " << packet.id << "\n";
						response.x = 0;
						send(clientSocket, (char*)&response, sizeof(GamePacket), 0);
					}
				}
			}

			if (connClients.size() >= 2)
			{
				SOCKET p1 = connClients[0].first;
				SOCKET p2 = connClients[1].first;
				std::string p1ID = connClients[0].second;
				std::string p2ID = connClients[1].second;

				std::cout << "[서버] 2명이 모여 새로운 게임 방을 생성합니다. \n";

				std::thread roomThread(&ServerManager::GameRoomThread, this, p1, p2, p1ID, p2ID);

				roomThread.detach();

				connClients.erase(connClients.begin());
				connClients.erase(connClients.begin());
			}
		}
	}
}

void ServerManager::GameRoomThread(SOCKET player1, SOCKET player2, std::string p1ID, std::string p2ID)
{
	int black = 1;
	int white = 2;

	send(player1, (char*)&black, sizeof(int), 0);
	send(player2, (char*)&white, sizeof(int), 0);

	std::cout << "[서버 방] 새로운 게임이 시작되었습니다" << std::endl;

	GamePacket packet;

	int result;

	while (true)
	{
		result = recv(player1, (char*)&packet, sizeof(GamePacket), 0);

		if (result <= 0)
		{
			std::cout << "[서버 방] Player 1 연결 끊김. 게임 종료" << "\n";
			break;
		}

		send(player2, (char*)&packet, sizeof(GamePacket), 0);

		if (packet.type == 1)
		{
			std::cout << "[서버 방] Player 1 승리!" << "\n";

			db.UpdateRecord(p1ID, true);
			db.UpdateRecord(p2ID, false);

			break;
		}

		result = recv(player2, (char*)&packet, sizeof(GamePacket), 0);

		if (result <= 0)
		{
			std::cout << "[서버 방] Player 2 연결 끊김. 게임 종료" << "\n";
			break;
		}

		send(player1, (char*)&packet, sizeof(GamePacket), 0);

		if (packet.type == 1)
		{
			std::cout << "[서버 방] Player 2 승리!" << "\n";

			db.UpdateRecord(p1ID, false);
			db.UpdateRecord(p2ID, true);

			break;
		}

		Sleep(10);
	}

	std::cout << "[서버 방] 방 닫음" << std::endl;
	closesocket(player1);
	closesocket(player2);
}

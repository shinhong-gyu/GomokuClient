#pragma once
#include <winsock2.h>
#include <vector>
#include <string>

#include "../DB/DBManager.h"

#pragma comment(lib, "ws2_32.lib")

enum PacketType { LOGIN = 1, STONE = 2, WIN = 3, LEAVE = 4 };

struct GamePacket {
	int type;
	int x, y;
	char id[20];
	char pw[50];
};

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	bool StartServer(int port);

	void AcceptClients();

	void GameRoomThread(SOCKET player1, SOCKET player2, std::string p1ID, std::string p2ID);

private:
	SOCKET serverSocket;
	std::vector<std::pair<SOCKET, std::string>> connClients;

	DBManager db;
};

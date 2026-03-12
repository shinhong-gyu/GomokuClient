#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <queue>

#pragma comment(lib, "ws2_32.lib")

enum PacketType { LOGIN = 1, STONE = 2, WIN = 3, LEAVE = 4, HEARTBEAT = 5, MATCHING = 6, SIGNIN = 7 };

struct GamePacket
{
	int type;
	int x, y;
	char id[20];
	char pw[50];
};

struct GameInfo
{
	char oppID[20];
	int win;
	int lose;
	int color;
};

class Player
{
public:
	Player();
	~Player();

	bool ConnetToServer(const char* ip, int port);

	int SendPacket(GamePacket packet);
	void RecvPacket(GamePacket& packet);

	void WaitingGame();

	bool TryLogin();
	bool TrySignIn(std::string id, std::string pw);

	void PacketReceiver();

	void GameEnd();

	SOCKET sock;

	int color;

	std::string id;
	std::string pw;

	bool bIsMyTurn = false;

private:
	bool bIsGameEnd = false;
	std::queue<GamePacket> recvQueue;

	bool bIsReceiverRunning = false;
};
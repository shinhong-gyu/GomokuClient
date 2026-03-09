#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

#include "../Game/Game.h"

#pragma comment(lib, "ws2_32.lib")

enum PacketType { LOGIN = 1, STONE = 2, WIN = 3, LEAVE = 4 };

struct GamePacket 
{
	int type;
	int x, y;
	char id[20];
	char pw[50];
};


class Player
{
	friend class Game;
public:
	Player();

	bool ConnetToServer(const char* ip, int port);

	void SendPacket(GamePacket packet);
	void RecvPacket(GamePacket& packet);

	void WaitingGame();

	bool TryLogin();
private:
	SOCKET sock;

	int color;

	std::string id;
	std::string pw;

	bool bIsMyTurn = false;
};
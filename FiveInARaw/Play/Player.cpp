#include "Player.h"

#include <iostream>
#include <cstring>

Player::Player()
{

}

bool Player::ConnetToServer(const char* ip, int port)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &serverAddr.sin_addr);

	if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != SOCKET_ERROR)
	{
		return true;
	}

	return false;
}

void Player::SendPacket(GamePacket packet)
{
	send(sock, (char*)&packet, sizeof(GamePacket), 0);
}

void Player::RecvPacket(GamePacket& packet)
{
	recv(sock, (char*)&packet, sizeof(GamePacket), 0);
}

void Player::WaitingGame()
{
	std::cout << "[클라이언트] 상대 찾는중 ..." << "\n";

	recv(sock, (char*)&color, sizeof(int), 0);

	bIsMyTurn = (color == 1);

	if (bIsMyTurn)
	{
		std::cout << "[클라이언트] 흑돌(선공)" << "\n";
	}
	else
	{
		std::cout << "[클라이언트] 백돌(후공)" << "\n";
	}
}

bool Player::TryLogin()
{
	std::cout << "[로그인] ID : ";
	std::cin >> id;

	std::cout << "[로그인] PW : ";
	std::cin >> pw;

	GamePacket loginPacket = {};

	strncpy_s(loginPacket.id, sizeof(loginPacket.id), id.c_str(), _TRUNCATE);
	loginPacket.id[sizeof(loginPacket.id) - 1] = '\0';
	strncpy_s(loginPacket.pw, sizeof(loginPacket.pw), pw.c_str(), _TRUNCATE);
	loginPacket.pw[sizeof(loginPacket.id) - 1] = '\0';

	loginPacket.type = PacketType::LOGIN;

	SendPacket(loginPacket);

	GamePacket response = {};
	RecvPacket(response);

	if (response.type == LOGIN && response.x == 1)
	{
		std::cout << "[클라이언트] 로그인 성공!" << std::endl;
		Sleep(1000);
		return true;
	}
	else
	{
		std::cout << "[클라이언트] 로그인 실패" << std::endl;
		Sleep(1000);
		return false;
	}
}

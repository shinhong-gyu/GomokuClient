#include "Player.h"

#include <iostream>
#include <cstring>

#include <thread>
#include <chrono>

Player::Player()
{
	color = 1;
	sock = INVALID_SOCKET;
}

bool Player::ConnetToServer(const char* ip, int port)
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result)
	{
		std::cout << "[클라이언트] WSAStartup 실패: " << result << "\n";
		return false;
	}

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

int Player::SendPacket(GamePacket packet)
{
	return send(sock, (char*)&packet, sizeof(GamePacket), 0);
}

void Player::RecvPacket(GamePacket& packet)
{
	recv(sock, (char*)&packet, sizeof(GamePacket), 0);
}	


void Player::WaitingGame()
{
	GameInfo infoPacket = {};

	std::cout << "[클라이언트] 상대 찾는중 ..." << "\n";

	recv(sock, (char*)&infoPacket, sizeof(GameInfo), 0);

	system("cls");

	color = infoPacket.color;

	bIsMyTurn = (infoPacket.color == 1);

	if (bIsMyTurn)
	{
		std::cout << "[클라이언트] 흑돌(선공)" << "\n";
	}
	else
	{
		std::cout << "[클라이언트] 백돌(후공)" << "\n";
	}

	std::cout << "[클라이언트] 상대 ID : " << std::string(infoPacket.oppID) << "\n";
	std::cout << "[클라이언트] 상대 전적 : " << infoPacket.win << "승 " << infoPacket.lose << "패" << "\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	system("cls");
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

	std::cout << "[로그인] 로그인 시도 중... " << "\n";

	RecvPacket(response);

	if (response.type == LOGIN && response.x == 1)
	{
		std::cout << "[클라이언트] 로그인 성공!" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		return true;
	}

	if (response.type == LOGIN && response.x == 0)
	{
		std::cout << "[클라이언트] 로그인 실패" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		return false;
	}

	if (response.type == LOGIN && response.x == -1)
	{
		std::cout << "[클라이언트] 이미 로그인 된 아이디입니다." << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		if (ConnetToServer("127.0.0.1", 9000))
		{
			return TryLogin();
		}

		std::cout << "[클라이언트] 재연결 실패" << "\n";

	}

	return false;
}

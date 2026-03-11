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

	system("cls");

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
	system("cls");

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
		std::cout << "[클라이언트] 로그인 성공!" << "\n";

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		return true;
	}

	std::string str = response.x >= 0 ? id + "는 존재하지 않는 아이디입니다." : (response.x == -1) ? "이미 로그인 된 아이디입니다." : "비밀번호가 일치하지 않습니다";

	std::cout << "[클라이언트] " << str << "\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	if (ConnetToServer("127.0.0.1", 9000))
	{
		if (response.x == 0)
		{
			return TrySignIn(id, pw);
		}

		return TryLogin();
	}

	std::cout << "[클라이언트] 재연결 실패" << "\n";

	return false;
}

bool Player::TrySignIn(std::string id, std::string pw)
{
	system("cls");

	std::cout << "[회원가입] ID : " << id << "\n";
	std::cout << "[회원가입] PW : " << pw << "\n";

	std::cout << "[회원가입] 이대로 가입하시겠습니까?" << "\n";
	std::cout << "\t[Y] 예\t[N] 아니오" << "\n";

	char yesORno;

	std::cin >> yesORno;

	if (yesORno == 'Y' || yesORno == 'y')
	{
		GamePacket packet = {};

		strncpy_s(packet.id, sizeof(packet.id), id.c_str(), _TRUNCATE);
		packet.id[sizeof(packet.id) - 1] = '\0';
		strncpy_s(packet.pw, sizeof(packet.pw), pw.c_str(), _TRUNCATE);
		packet.pw[sizeof(packet.id) - 1] = '\0';

		packet.type = PacketType::SIGNIN;

		send(sock, (char*)&packet, sizeof(GamePacket), 0);

		GamePacket response = {};

		recv(sock, (char*)&response, sizeof(GamePacket), 0);

		if (response.x == 1)
		{
			std::cout << "[클라이언트] 회원가입 성공!" << "\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));

			ConnetToServer("127.0.0.1", 9000);

			return TryLogin();
		}

	}
	else if (yesORno == 'N' || yesORno == 'n')
	{
		std::cout << "[회원가입] ID 입력 : ";
		std::cin >> id;


		std::cout << "[회원가입] PW 입력 : ";
		std::cin >> pw;

		return TrySignIn(id, pw);
	}

	return true;
}

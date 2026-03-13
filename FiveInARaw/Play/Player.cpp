#include "Player.h"

#include <iostream>
#include <cstring>

#include <thread>
#include <chrono>
#include <mutex>

std::mutex mtx;

Player::Player()
{
	color = 1;
	sock = INVALID_SOCKET;
}

Player::~Player()
{
	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
	}

	WSACleanup();
}

bool Player::ConnetToServer()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result)
	{
		std::cout << "[클라이언트] WSAStartup 실패: " << result << "\n";
		return false;
	}

	if (sock != INVALID_SOCKET)
	{
		closesocket(sock);
		sock = INVALID_SOCKET;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);

	std::cout << "[클라이언트] 서버 소켓 : " << sock << "\n";

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

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
	while (true)
	{
		if (recvQueue.size() != 0)
		{
			std::lock_guard<std::mutex> lock(mtx);

			packet = recvQueue.front();

			recvQueue.pop();

			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void Player::WaitingGame()
{
	bIsGameEnd = false;

	if (!bIsReceiverRunning)
	{
		bIsReceiverRunning = true;
		std::thread receiverThread(&Player::PacketReceiver, this);
		receiverThread.detach();
	}

	system("cls");

	std::cout << "[클라이언트] 상대 찾는중 ..." << "\n";

	GamePacket packet = {};

	RecvPacket(packet);

	while (packet.type == PacketType::LEAVE)
	{
		RecvPacket(packet);
	}

	GameInfo* infoPacket = (GameInfo*)&packet;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	system("cls");

	color = infoPacket->color;

	bIsMyTurn = (infoPacket->color == 1);

	if (bIsMyTurn)
	{
		std::cout << "[클라이언트] 흑돌(선공)" << "\n";
	}
	else
	{
		std::cout << "[클라이언트] 백돌(후공)" << "\n";
	}

	std::cout << "[클라이언트] 상대 ID : " << std::string(infoPacket->oppID) << "\n";
	std::cout << "[클라이언트] 상대 전적 : " << infoPacket->win << "승 " << infoPacket->lose << "패" << "\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(1500));

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

	recv(sock, (char*)&response, sizeof(GamePacket), 0);

	if (response.type == LOGIN && response.x == 1)
	{
		std::cout << "[클라이언트] 로그인 성공!" << "\n";

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		return true;
	}

	std::string str = response.x >= 0 ? id + "는 존재하지 않는 아이디입니다." : (response.x == -1) ? "이미 로그인 된 아이디입니다." : "비밀번호가 일치하지 않습니다";

	std::cout << "[클라이언트] " << str << "\n";

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	if (ConnetToServer())
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
	std::cout << "[Y] 예\t[N] 아니오\t[R] 다른 아이디로 로그인" << "\n";

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

		SendPacket(packet);

		GamePacket response = {};

		recv(sock, (char*)&response, sizeof(GamePacket), 0);

		if (response.x == 1)
		{
			std::cout << "[클라이언트] 회원가입 성공!" << "\n";
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));

			ConnetToServer();

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
	else if (yesORno == 'R' || yesORno == 'r')
	{
		return TryLogin();
	}

	return false;
}

void Player::PacketReceiver()
{
	GamePacket packet;

	while (true)
	{
		int result = 0;

		result = recv(sock, (char*)&packet, sizeof(GamePacket), 0);

		if (result <= 0)
		{
			bIsReceiverRunning = false;
			break;
		}

		if (result != SOCKET_ERROR)
		{
			std::lock_guard<std::mutex> lock(mtx);

			recvQueue.push(packet);
		}

		if (bIsGameEnd)
		{
			bIsReceiverRunning = false;
			break;
		}
	}
}

void Player::GameEnd()
{
	bIsReceiverRunning = false;
	bIsGameEnd = true;
}

#include "Game.h"

#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>

Game::Game()
{
	if (player.ConnetToServer("127.0.0.1", 9000))
	{
		std::cout << "[클라이언트] 서버 연결 성공";

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));
		system("cls");

		if (player.TryLogin())
		{
			system("cls");

			player.WaitingGame();

			Board.DrawBoard();
		}
		else
		{
			system("pause");
		}
	}
	else
	{
		std::cout << "[클라이언트] 서버 연결 실패" << "\n";
	}
}

Game::~Game()
{
}

void Game::Run()
{
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	int64_t currentTime = time.QuadPart;
	int64_t previousTime = currentTime;

	// Game-Loop.
	while (true)
	{
		if (quit)
		{
			break;
		}

		QueryPerformanceCounter(&time);
		currentTime = time.QuadPart;

		float deltaTime = static_cast<float>(currentTime - previousTime) / static_cast<float>(frequency.QuadPart);

		targetOneFrameTime = 1.0f / targetFrameRate;

		if (deltaTime >= targetOneFrameTime)
		{
			if (player.bIsMyTurn)
			{
				if (ProcessInput())
				{
					if (Board.PutStone(mousePosition.x, mousePosition.y, player.color))
					{
						UI.Gotoxy(0, 0);

						Board.DrawBoard();

						GamePacket packet = {};

						if (Board.CheckWin(mousePosition.x / 2, mousePosition.y, player.color))
						{
							std::cout << "\t\tYou Win" << std::endl;

							packet.type = PacketType::WIN;

							packet.x = mousePosition.x;
							packet.y = mousePosition.y;

							strncpy_s(packet.id, sizeof(packet.id), player.id.c_str(), _TRUNCATE);
							packet.id[sizeof(packet.id) - 1] = '\0';
							strncpy_s(packet.pw, sizeof(packet.pw), player.pw.c_str(), _TRUNCATE);
							packet.pw[sizeof(packet.id) - 1] = '\0';

							int result = player.SendPacket(packet);

							if (result == SOCKET_ERROR)
							{
								OpponentLeave();
								break;
							}

							std::this_thread::sleep_for(std::chrono::milliseconds(3000));

							ShowOption();
						}
						else
						{
							packet.type = PacketType::STONE;

							packet.x = mousePosition.x;
							packet.y = mousePosition.y;

							strncpy_s(packet.id, sizeof(packet.id), player.id.c_str(), _TRUNCATE);
							packet.id[sizeof(packet.id) - 1] = '\0';
							strncpy_s(packet.pw, sizeof(packet.pw), player.pw.c_str(), _TRUNCATE);
							packet.pw[sizeof(packet.id) - 1] = '\0';

							int result = player.SendPacket(packet);

							player.bIsMyTurn = false;
						}
					}
				}
			}
			else
			{
				GamePacket packet;

				player.RecvPacket(packet);

				switch (packet.type)
				{
				case PacketType::WIN:
					UI.Gotoxy(0, 0);

					Board.PutStone(packet.x, packet.y, (player.color == 1) ? 2 : 1);

					Board.DrawBoard();

					std::cout << "\t\tYou Lose" << std::endl;

					std::this_thread::sleep_for(std::chrono::milliseconds(2000));

					ShowOption();

					break;

				case PacketType::LEAVE:
					OpponentLeave();

					break;

				case PacketType::STONE:
					UI.Gotoxy(0, 0);

					Board.PutStone(packet.x, packet.y, (player.color == 1) ? 2 : 1);

					Board.DrawBoard();

					player.bIsMyTurn = true;
					break;

				default:
					break;
				}
			}
		}
	}
}

bool Game::ProcessInput()
{
	HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	FlushConsoleInputBuffer(inputHandle);

	static int flag = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_EXTENDED_FLAGS;
	SetConsoleMode(inputHandle, flag);


	INPUT_RECORD record;
	DWORD events;

	if (PeekConsoleInput(inputHandle, &record, 1, &events) && events > 0)
	{
		if (ReadConsoleInput(inputHandle, &record, 1, &events))
		{
			switch (record.EventType)
			{
			case KEY_EVENT:
			{
				if (record.Event.KeyEvent.bKeyDown)
				{
					keyState[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = true;
				}

				else
				{
					keyState[record.Event.KeyEvent.wVirtualKeyCode].isKeyDown = false;
				}
				break;
			}

			case MOUSE_EVENT:
			{
				MOUSE_EVENT_RECORD mouse = record.Event.MouseEvent;

				mousePosition.x = record.Event.MouseEvent.dwMousePosition.X;
				mousePosition.y = record.Event.MouseEvent.dwMousePosition.Y;

				bool isLeftDown = (mouse.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;

				if (isLeftDown)
				{
					keyState[VK_LBUTTON].isKeyDown = isLeftDown;

					//keyState[VK_RBUTTON].isKeyDown = (record.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;

					return true;
				}

				break;
			}
			}
		}
	}

	return false;
}

void Game::CloseGame()
{
	Board.InitGameBoard();

	player.bIsMyTurn = false;
}

void Game::OpponentLeave()
{
	system("cls");

	std::cout << "[클라이언트] 상대와 연결이 끊어졌습니다. 게임 종료" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	ShowOption();
}

void Game::ShowOption()
{
	system("cls");

	std::cout << "\t[R] 상대찾기\t[Q] 종료하기" << "\n";

	while (true)
	{
		ProcessInput();

		if (keyState['R'].isKeyDown == true)
		{
			CloseGame();

			GamePacket packet = {};
			packet.type = PacketType::MATCHING;

			int result = player.SendPacket(packet);

			keyState['R'].isKeyDown = false;

			if (result != SOCKET_ERROR)
			{
				player.WaitingGame();

				Board.DrawBoard();
			}
			else
			{
				system("cls");

				std::cout << "[클라이언트] 입력이 없어 서버와 연결이 끊겼습니다.\n[클라이언트] 다시 로그인 하세요" << "\n";

				if (player.ConnetToServer("127.0.0.1", 9000))
				{
					std::cout << "[클라이언트] 서버 재연결 성공" << "\n";

					std::this_thread::sleep_for(std::chrono::milliseconds(3000));

					system("cls");

					if (player.TryLogin())
					{
						system("cls");

						player.WaitingGame();

						Board.DrawBoard();
					}
					else
					{
						system("pause");
					}
				}
				else
				{
					std::cout << "[클라이언트] 서버 연결 실패" << "\n";

					quit = true;
				}
			}

			break;
		}

		if (keyState['Q'].isKeyDown == true)
		{
			CloseGame();
			quit = true;
			break;
		}
	}

}



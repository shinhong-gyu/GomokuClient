#include "Game.h"

#include <Windows.h>
#include <iostream>

Game::Game()
{
	Board.DrawBoard();
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

		if (deltaTime >= targetOneFrameTime)
		{
			if (ProcessInput())
			{
				if (Board.PutStone(mousePosition.x, mousePosition.y, turn))
				{
					system("cls");

					Board.DrawBoard();

					if (Board.CheckWin(mousePosition.x/2, mousePosition.y, turn))
					{
						system("cls");
						std::cout << "Player" << turn << " win" << std::endl;
						quit = true;
					}
					else
					{
						turn = turn == 1 ? 2 : 1;
					}
				}
				else
				{
					// 둘 수 없는 곳일 때 처리
				}
			}
		}
	}
}

bool Game::ProcessInput()
{
	HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	static int flag = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_EXTENDED_FLAGS;
	SetConsoleMode(inputHandle, flag);

	INPUT_RECORD record;
	DWORD events;

	if (PeekConsoleInput(inputHandle, &record, 1, &events) && events > 0)
	{
		if (record.EventType == WINDOW_BUFFER_SIZE_EVENT)
		{
			FlushConsoleInputBuffer(inputHandle);
		}

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
			}
			break;

			case MOUSE_EVENT:
			{
				MOUSE_EVENT_RECORD mouse = record.Event.MouseEvent;
				if (mouse.dwEventFlags == 0)
				{
					mousePosition.x = record.Event.MouseEvent.dwMousePosition.X;
					mousePosition.y = record.Event.MouseEvent.dwMousePosition.Y;

					keyState[VK_LBUTTON].isKeyDown
						= (record.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;

					keyState[VK_RBUTTON].isKeyDown
						= (record.Event.MouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;

					if (keyState[VK_LBUTTON].isKeyDown)
					{
						return true;
					}
				}

			}
			break;

			}
		}
	}

	return false;
}
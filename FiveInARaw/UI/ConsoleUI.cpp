#include "ConsoleUI.h"

ConsoleUI::ConsoleUI()
{
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTitle(TEXT("C++ 1:1 오목 게임"));

	SetConsoleSize(60,25);

	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(hOut, &cursorInfo);

	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(hOut, &cursorInfo);

	DWORD consoleMode;
	GetConsoleMode(hIn, &consoleMode);

	consoleMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

	SetConsoleMode(hIn, consoleMode);
}   

void ConsoleUI::SetConsoleSize(int Width, int Height) 
{
	SMALL_RECT minWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(hOut, TRUE, &minWindow);

	COORD bufferSize = { (SHORT)Width, (SHORT)Height };
	SetConsoleScreenBufferSize(hOut, bufferSize);

	SMALL_RECT windowSize = { 0, 0, (SHORT)(Width - 1), (SHORT)(Height - 1) };
	SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}


void ConsoleUI::GetMouseClick(int& x, int& y)
{
}

void ConsoleUI::Gotoxy(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, pos);
}

#include "ConsoleUI.h"

ConsoleUI::ConsoleUI()
{
	HIn = GetStdHandle(STD_INPUT_HANDLE);
	HOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTitle(TEXT("C++ 1:1 오목 게임"));

	SetConsoleSize(60,25);

	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(HOut, &CursorInfo);

	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(HOut, &CursorInfo);

	DWORD ConsoleMode;
	GetConsoleMode(HIn, &ConsoleMode);

	ConsoleMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;

	SetConsoleMode(HIn, ConsoleMode);
}   

void ConsoleUI::SetConsoleSize(int Width, int Height) 
{
	SMALL_RECT minWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(HOut, TRUE, &minWindow);

	COORD bufferSize = { (SHORT)Width, (SHORT)Height };
	SetConsoleScreenBufferSize(HOut, bufferSize);

	SMALL_RECT windowSize = { 0, 0, (SHORT)(Width - 1), (SHORT)(Height - 1) };
	SetConsoleWindowInfo(HOut, TRUE, &windowSize);
}


void ConsoleUI::GetMouseClick(int& x, int& y)
{
}

void ConsoleUI::Gotoxy(int x, int y)
{
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(HOut, pos);
}

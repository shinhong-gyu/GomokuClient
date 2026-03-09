#pragma once
#include <windows.h>
#include <iostream>

class ConsoleUI
{
public:
	ConsoleUI();

	~ConsoleUI() = default;

	void GetMouseClick(int& x, int& y);

	void SetConsoleSize(int width, int height);

	void Gotoxy(int x, int y);
private:
	HANDLE hIn;
	HANDLE hOut;
};
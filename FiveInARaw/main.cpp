#include <iostream>

#include "UI/ConsoleUI.h"
#include "Play/GomokuBoard.h"

using namespace std;

int main()
{
	ConsoleUI UI;

	GomokuBoard Board;

	while (1)
	{
		Board.DrawBoard();
	}
	
}
#include "GomokuBoard.h"
#include <iostream>


using namespace std;

GomokuBoard::GomokuBoard()
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			Board[y][x] = EMPTY;
		}
	}
}

void GomokuBoard::DrawBoard()
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			if (Board[y][x] == BLACK)
			{
				cout << "≒";
			}
			else if (Board[y][x] == WHITE)
			{
				cout << "∞";
			}
			else {
				if (y == 0 && x == 0)
				{
					cout << "忙式";
				}
				else if (y == 0 && x == BOARD_SIZE - 1)
				{
					cout << "忖";
				}
				else if (y == BOARD_SIZE - 1 && x == 0)
				{
					cout << "戌式";
				}
				else if (y == BOARD_SIZE - 1 && x == BOARD_SIZE - 1)
				{
					cout << "戎";
				}
				else if (y == 0)
				{
					cout << "成式";
				}
				else if (y == BOARD_SIZE - 1)
				{
					cout << "扛式";
				}
				else if (x == 0)
				{
					cout << "戍式";
				}
				else if (x == BOARD_SIZE - 1)
				{
					cout << "扣";
				}
				else
				{
					cout << "托式";
				}
			}
		}
		cout << endl;
	}
}

bool GomokuBoard::PutStone(int X, int Y, int Player)
{
	return false;
}

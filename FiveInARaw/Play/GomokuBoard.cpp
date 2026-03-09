#include "GomokuBoard.h"
#include <iostream>

#include "Player.h"

using namespace std;

GomokuBoard::GomokuBoard()
{
	InitGameBoard();
}

void GomokuBoard::InitGameBoard()
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			board[y][x] = EMPTY;
		}
	}
}

void GomokuBoard::DrawBoard()
{
	for (int y = 0; y < BOARD_SIZE; y++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			if (board[y][x] == BLACK)
			{
				cout << "≒";
			}
			else if (board[y][x] == WHITE)
			{
				cout << "∞";
			}
			else
			{
				if ((y == 3 || y == 9 || y == 15) && (x == 3 || x == 9 || x == 15))
				{
					cout << "朱式";
					continue;
				}
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

bool GomokuBoard::PutStone(int x, int y, int color)
{
	if (board[y][x / 2] != 0)
	{
		return false;
	}

	if (x >= BOARD_SIZE * 2 || x <= 0)
	{
		return false;
	}

	if (y >= BOARD_SIZE || y < 0)
	{
		return false;
	}

	board[y][x / 2] = color;

	return true;
}

bool GomokuBoard::CheckWin(int x, int y, int player)
{
	int dx[] = { 1, 0, 1, 1 };
	int dy[] = { 0, 1, 1, -1 };

	for (int i = 0; i < 4; i++)
	{
		int count = 1;

		for (int dir = -1; dir <= 1; dir += 2)
		{
			int nx = x;
			int ny = y;

			while (true)
			{
				nx += (dx[i] * dir);
				ny += (dy[i] * dir);

				if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE)
				{
					break;
				}
				if (board[ny][nx] != player)
				{
					break;
				}

				count++;
			}
		}

		if (count >= 5)
		{
			return true;
		}
	}

	return false;
}

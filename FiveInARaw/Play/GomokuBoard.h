#pragma once

const int BOARD_SIZE = 19;
const int EMPTY = 0;
const int BLACK = 1;
const int WHITE = 2;

class GomokuBoard
{
public:
	GomokuBoard();

	void DrawBoard();

	bool PutStone(int x, int y, int player);

	bool CheckWin(int x, int y, int player);

private:
	int board[BOARD_SIZE][BOARD_SIZE];
};
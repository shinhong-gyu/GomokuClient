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

	bool PutStone(int X, int Y, int Player);

private:
	int Board[BOARD_SIZE][BOARD_SIZE];
};
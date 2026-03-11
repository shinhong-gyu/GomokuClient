#pragma once
#include "../UI/ConsoleUI.h"
#include "../Play/GomokuBoard.h"
#include "../Play/Player.h"

struct KeyState
{
	bool isKeyDown = false;

	bool wasKeyDowm = false;
};

struct Vector2
{
	int x = 0;
	int y = 0;
};

class Game
{
public:
	Game();
	~Game();

	void Run();

	bool ProcessInput();

	void CloseGame();

	void OpponentLeave();

private:
	float targetFrameRate = 60.0f;

	float targetOneFrameTime = 0.0f;

	bool quit = false;

	bool bHasChange = false;

	KeyState keyState[255];

	Vector2 mousePosition;

	ConsoleUI UI;

	GomokuBoard Board;

	int turn = 1;

	Player player;

	float heartbeatTime = 5.0f;
};
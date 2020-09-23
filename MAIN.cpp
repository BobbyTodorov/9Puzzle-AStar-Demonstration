#include "GameModel.h"

int main()
{
	Game game;
	int goalArr[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 0} };
	game.SetGoalState(goalArr);
	game.AStar();
	system("pause");
	return 0;
}
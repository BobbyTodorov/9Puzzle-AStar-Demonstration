#ifndef _GAMEMODEL_HEADER_INCLUDED_
#define _GAMEMODEL_HEADER_INCLUDED_

#include <iostream>
#include <list>
#include <algorithm> 
#include <ctime>
#include <cmath>

using namespace std;

const size_t BOARD_SIZE = 3; //3x3

class State
{
	bool IsSolvable();
	int GetInversionsCount();
	void InitializeState();

public:
	int g, h, t; //General Cost, Heuristic Cost, Total Cost  
	State* parent;
	int board[BOARD_SIZE][BOARD_SIZE];

	State();
	void PrintBoard() const;
	bool IsGoalState() const;
	bool operator==(const State&) const; //compares the boards of two states
	bool operator<(const State&) const; //compares states' total costs
	void Heurs();
};

class Game
{
	list<State> inactiveStates, activeStates;
	State start, cur, temp;
	unsigned pathLength;

	bool IsGoalStateReached() const;
	bool IsInactiveState(State& s);
	void ExpandCurrentState();
	void PrintPath(State*);
	void InitializeGame();

public:
	Game();
	void SetGoalState(int arr[BOARD_SIZE][BOARD_SIZE]);
	void AStar();
};

#endif // !_GAMEMODEL_HEADER_INCLUDED

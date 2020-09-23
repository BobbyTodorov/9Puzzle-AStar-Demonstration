#include "GameModel.h"
bool generateStartState = true;
int goal[BOARD_SIZE][BOARD_SIZE];

int State::GetInversionsCount()
{
	const size_t LINEAR_STATE_LENGTH = BOARD_SIZE * BOARD_SIZE;
	int linearState[LINEAR_STATE_LENGTH];
	size_t lineageStateIdx = 0;
	for(size_t i=0; i<BOARD_SIZE;++i)
		for (size_t j = 0; j < BOARD_SIZE; ++j)
		{
			linearState[lineageStateIdx] = board[i][j];
			lineageStateIdx++;
		}

	int invCount = 0;
	size_t maxIter = BOARD_SIZE * BOARD_SIZE - 1;
	for (size_t i = 0; i < maxIter; ++i)
	{
		for (size_t j = i + 1; j <= maxIter; ++j)
		{
			if (linearState[j] && linearState[i] && linearState[i] > linearState[j])
				invCount = invCount + 1;
		}
	}
	return invCount;
}
 
bool State::IsSolvable()
{
	int invCount = GetInversionsCount();
	return (invCount % 2 == 0);
}

State::State()
{
	InitializeState();
}

void State::InitializeState()
{
	g = h = t = 0;
	parent = nullptr;
	if (generateStartState)
	{
		list<int> l;
		list<int>::iterator it;
		srand((unsigned)time(0));
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; ++i)
			l.push_back(i);
		for (int i = 0; i < BOARD_SIZE; ++i) 
		{
			for (int j = 0; j < BOARD_SIZE; ++j) 
			{
				it = l.begin();
				for (int k = rand() % l.size(); k > 0; --k)
					it++;
				board[i][j] = (*it);
				l.erase(it);
			}
		}
		generateStartState = false;
		if (!IsSolvable())
		{
			std::cout << "No solution for: " << endl;
			PrintBoard();
			exit(0);
		}
	}
}

void State::PrintBoard() const
{
	for (size_t i = 0; i < BOARD_SIZE; ++i)
		cout << board[i][0] << " " << board[i][1] << " " << board[i][2] << std::endl;
}

bool State::IsGoalState() const 
{
	for (size_t i = 0; i < BOARD_SIZE; ++i) 
	{
		for (size_t j = 0; j < BOARD_SIZE; ++j) 
		{
			if (board[i][j] != goal[i][j])
				return false;
		}
	}

	return true;
}

bool State::operator==(const State& rhs) const 
{
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		for (size_t j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] != rhs.board[i][j])
				return false;
		}
	}

	return true;
}

bool State::operator<(const State& rhs) const 
{
	return t < rhs.t;
}

void State::Heurs()
{
	//Heuristic function that works based on Manhattan distance as explained in the task.
	int heuristic = 0;
	bool found;
	for (size_t i = 0; i < BOARD_SIZE; i++) 
	{
		for (size_t j = 0; j < BOARD_SIZE; j++) 
		{
			found = false;
			for (size_t k = 0; k < BOARD_SIZE; k++) 
			{
				for (size_t l = 0; l < BOARD_SIZE; l++) 
				{
					if (goal[i][j] == board[k][l]) 
					{
						heuristic += abs((int)(i - k)) + abs((int)(j - l)); //Manhattan distance
						found = true;
					}
					if (found)
						break;
				}
				if (found)
					break;
			}
		}
	}
	h = heuristic;
}

void Game::PrintPath(State* s) //recursive
{
	if (s != NULL) 
	{
		pathLength++;
		s->PrintBoard();
		if (s->parent)
			cout << "  ^" << endl << "  |" << endl;
		PrintPath(s->parent);
	}
}

bool Game::IsGoalStateReached() const
{
	return cur.IsGoalState();
}

Game::Game() : pathLength(0)
{
	InitializeGame();
}

void Game::SetGoalState(int arr[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; ++i)
		for (int j = 0; j < BOARD_SIZE; ++j)
			goal[i][j] = arr[i][j];
}

void Game::AStar() // A* search algorithm function
{
	cout << "Running A*:" << endl;
	cur = start;
	activeStates.push_front(cur);
	while (true) 
	{
		cur = activeStates.front();
		for (list<State>::iterator it = activeStates.begin(); it != activeStates.end(); ++it) 
			if ((*it) < cur) //find minimal total cost
				cur = (*it);

		if (IsGoalStateReached()) 
		{
			cout << "Path:" << endl;
			pathLength = 0;
			PrintPath(&cur);
			cout << "Path Length: " << pathLength << endl;
			return;
		}
		else
			ExpandCurrentState();
	}
}

void Game::InitializeGame()
{
	start.g = 0;
	start.t = start.g + start.h;
	start.parent = NULL;
}

void Game::ExpandCurrentState() //moves logic
{
	inactiveStates.push_back(cur);
	for (size_t i = 0; i < BOARD_SIZE; ++i)
	{
		for (size_t j = 0; j < BOARD_SIZE; ++j)
		{
			//finding the 0 block
			if (cur.board[i][j] == 0)
			{
				if (i > 0) //if the 0 is not in the first row
				{
					//set the child basic elements
					temp = cur;
					temp.parent = &(inactiveStates.back());
					//(to say "shifting the empty block (the zero element)" 
					//is the same as "shifting an adjecent block with number in 
					//it to the position of the empty block"), so

					//shift the zero element UP 
					swap(temp.board[i][j], temp.board[i - 1][j]);

					//search for the child in inactivelist
					if (!IsInactiveState(temp)) //if the child is not found in it
					{
						temp.g += 1;
						temp.Heurs();
						temp.t = temp.g + temp.h;
						activeStates.push_front(temp); //push the shild into the activelist
					}
				}
				
				if (i < BOARD_SIZE - 1) //if the 0 is not in the last row.
				{
					temp = cur;
					temp.parent = &(inactiveStates.back());
					//shift the zero element DOWN
					swap(temp.board[i][j], temp.board[i + 1][j]);
					if (!IsInactiveState(temp)) 
					{
						temp.g += 1;
						temp.Heurs();
						temp.t = temp.g + temp.h;
						activeStates.push_front(temp);
					}
				}
				if (j > 0) //if the 0 element not in the first column.
				{
					temp = cur;
					temp.parent = &(inactiveStates.back());
					//shift the zero element LEFT
					swap(temp.board[i][j], temp.board[i][j - 1]);
					if (!IsInactiveState(temp)) 
					{
						temp.g += 1;
						temp.Heurs();
						temp.t = temp.g + temp.h;
						activeStates.push_front(temp);
					}
				}
				if (j < BOARD_SIZE - 1) //if the zero elemnt not in the last column.
				{
					temp = cur;
					temp.parent = &(inactiveStates.back());
					//shift the zero element RIGHT
					swap(temp.board[i][j], temp.board[i][j + 1]);
					if (!IsInactiveState(temp)) {
						temp.g += 1;
						temp.Heurs();
						temp.t = temp.g + temp.h;
						activeStates.push_front(temp);
					}
				}
			}
		}
	}
	
	activeStates.remove(cur); //remove the expanded state from the activelist
}

bool Game::IsInactiveState(State& s)
{
	for (list<State>::iterator it = inactiveStates.begin(); it != inactiveStates.end(); ++it)
		if ((*it) == s)
			return true;
	return false;
}
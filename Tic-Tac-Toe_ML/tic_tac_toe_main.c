#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define BOARD_SIZE 3
#define EPISODES 10000
#define ALPHA 0.5
#define GAMMA 0.9
#define EPSILON 0.2
#define NUM_STATES 19683
#define NUM_ACTIONS 9
#define REWARD_WIN 1.0
#define REWARD_LOSE (-1.0)
#define REWARD_DRAW 0.5
#define EMPTY 0
#define X 1
#define O 2

typedef struct {
	int board[BOARD_SIZE][BOARD_SIZE];
}Board;

typedef struct {
	double q[NUM_STATES][NUM_ACTIONS];
	int player;
} Agent;

//Function to initialize the board
void initBoard(Board *board) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board->board[i][j] = EMPTY;
		}
	}
}

//Function to initialize the agent
void initAgent(Agent *agent) {
	for (int i = 0; i < NUM_STATES; i++) {
		for (int j = 0; j < NUM_ACTIONS; j++) {
			agent->q[i][j] = 0.0;
		}
	}
	agent->player = X;
}

//Function to print the board
void printBoard(Board *board) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board->board[i][j] == EMPTY) {
				printf(" ");
			}
			else if (board->board[i][j] == X) {
				printf("X");
			}
			else {
				printf("O");
			}
			if (j < BOARD_SIZE - 1) {
				printf("|");
			}
		}
		printf("\n");
		if (i < BOARD_SIZE - 1) {
			printf("-----\n");
		}
	}
}

//Function to update the q-values
void updateQ(Agent *agent, int state, int action, double reward, int next_state) {
	double max_next_q = agent->q[next_state][0];
	for (int i = 1; i < NUM_ACTIONS; i++) {
		if (agent->q[next_state][i] > max_next_q) {
			max_next_q = agent->q[next_state][i];
		}
	}
	agent->q[state][action] += ALPHA * (reward + GAMMA * max_next_q - agent->q[state][action]);
}

//Function to get the state
int getState(Board *board, int player) {
	int state = 0;
	int k = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board->board[i][j] == EMPTY) {
				state += 0 * pow(3, k);
			}
			else if (board->board[i][j] == player) {
				state += 1 * pow(3, k);
			}
			else {
				state += 2 * pow(3, k);
			}
			k++;
		}
	}
	return state;
}

//Function to update the board
void updateBoard(Board *board, int action, int player) {
	int i = action / BOARD_SIZE;
	int j = action % BOARD_SIZE;
	board->board[i][j] = player;
}

//Function to check if there is a winner
bool checkWin (Board * board){
	// Check rows and columns
	for (int i = 0; i < BOARD_SIZE; i++) {
		if ((board->board[i][0] == board->board[i][1] && board->board[i][1] == board->board[i][2] && board->board[i][0] != 0) ||
			(board->board[0][i] == board->board[1][i] && board->board[1][i] == board->board[2][i] && board->board[0][i] != 0)) {
			return (true);
		}
	}

	// Check diagonals
	if ((board->board[0][0] == board->board[1][1] && board->board[1][1] == board->board[2][2] && board->board[0][0] != 0) ||
		(board->board[0][2] == board->board[1][1] && board->board[1][1] == board->board[2][0] && board->board[0][2] != 0)) {
		return (true);
	}

	// No win state
	return (false);
}

//Function to check if there is a draw
bool checkDraw(Board *board) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board->board[i][j] == EMPTY) {
				return false;
			}
		}
	}
	return true;
}

//Function to check if the game is over
bool gameOver(Board *board, int player) {
	return (checkWin(board) || checkDraw(board));
}

//Function to get the reward
double getReward(Board *board, int player) {
	if (checkWin(board)) {
		if (player == X) {
			return REWARD_WIN;
		}
		else {
			return REWARD_LOSE;
		}
	}
	else if (checkDraw(board)) {
		return REWARD_DRAW;
	}
	else {
		return 0.0;
	}
}

//Function to get the best action following the epsilon-greedy policy
int getBestAction(Agent *agent, int state) {
	int best_action = 0;
	if ((double)rand() / (double)RAND_MAX < EPSILON) {
		best_action = rand() % NUM_ACTIONS;
	}
	else {
		double max_q = agent->q[state][0];
		for (int i = 1; i < NUM_ACTIONS; i++) {
			if (agent->q[state][i] > max_q) {
				max_q = agent->q[state][i];
				best_action = i;
			}
		}
	}
	return best_action;
}

//Function to get the next state
int getNextState(Board *board, int action, int player) {
	Board next_board;
	initBoard(&next_board);
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			next_board.board[i][j] = board->board[i][j];
		}
	}
	updateBoard(&next_board, action, player);
	return getState(&next_board, player);
}

//Function to train the agent
void train(Agent *agent) {
	Board board;
	initBoard(&board);
	for (int i = 0; i < EPISODES; i++) {
		initBoard(&board);
		int state = getState(&board, agent->player);
		while (!gameOver(&board, agent->player)) {
			updateBoard(&board, getBestAction(agent, state), agent->player);
			int next_state = getState(&board, agent->player);
			double reward = getReward(&board, agent->player);
			updateQ(agent, state, getBestAction(agent, state), reward, next_state);
			state = next_state;
			agent->player = (agent->player == X) ? O : X;
		}
	}
}

//Function to export the q-values to a .csv file
void exportQ(Agent *agent) {
	FILE *file = fopen("q_values.csv", "w");
	for (int i = 0; i < NUM_STATES; i++) {
		for (int j = 0; j < NUM_ACTIONS; j++) {
			fprintf(file, "%lf", agent->q[i][j]);
			if (j < NUM_ACTIONS - 1) {
				fprintf(file, ",");
			}
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

//Function to play against the agent
int main (){
	Agent agent;
	initAgent(&agent);
	Board board;
	initBoard(&board);
	train(&agent);
	exportQ(&agent);
	while (!gameOver(&board, agent.player)) {
		printBoard(&board);
		if (agent.player == X) {
			int action;
			printf("Enter the action: ");
			scanf("%d", &action);
			updateBoard(&board, action, agent.player);
		}
		else {
			int state = getState(&board, agent.player);
			int action = getBestAction(&agent, state);
			updateBoard(&board, action, agent.player);
		}
		agent.player = (agent.player == X) ? O : X;
	}
	printBoard(&board);
	agent.player = (agent.player == X) ? O : X;
	double reward = getReward(&board, agent.player);
	if (reward == REWARD_WIN) {
		printf("You win!\n");
	}
	else if (reward == REWARD_LOSE) {
		printf("You lose!\n");
	}
	else {
		printf("Draw!\n");
	}
	return 0;
}
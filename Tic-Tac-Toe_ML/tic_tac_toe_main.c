#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define BOARD_SIZE 3
#define EPISODES 10000
#define EPSILON 0.2
#define ALPHA 0.5
#define GAMMA 0.9
#define NUM_STATES 19683 // 3^9
#define NUM_ACTIONS 9
#define REWARD_WIN 1.0
#define REWARD_LOSE (-1.0)
#define REWARD_DRAW 0.5
#define EMPTY 0
#define X 1
#define O 2

typedef struct {
    int cells[BOARD_SIZE][BOARD_SIZE]; // Represents the state of the board, where 0 is an empty cell, 1 is an 'X' and 2 is an 'O'
} Board;

typedef struct {
    double q_table[NUM_STATES][NUM_ACTIONS]; // Q Table
    int player; // Current player
} Agent;

// Function to initialize agent and Q table
void initialize_agent(Agent *agent) {
    for (int i = 0; i < NUM_STATES; ++i) {
        for (int j = 0; j < NUM_ACTIONS; ++j) {
            agent->q_table[i][j] = 0.0; // Initialize all Q values to 0
        }
    }
    agent->player = X; // The agent plays as 'X'
}

// Function to initialize the board
void initialize_board(Board *board) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board->cells[i][j] = EMPTY;
        }
    }
}

// Dashboard display function
void display_board(Board *board) {
    printf("=====\n");
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            switch (board->cells[i][j]) {
                case EMPTY:
                    printf(" ");
                    break;
                case X:
                    printf("X");
                    break;
                case O:
                    printf("O");
                    break;
            }
            if (j < 2) {
                printf("|");
            }
        }
        printf("\n");
        if (i < 2) {
            printf("-----\n");
        }
    }
    printf("=====\n");
}

// Function to update Q values
void update_q_table(double q_table[NUM_STATES][NUM_ACTIONS], int state, int action, double reward, int next_state) {
    double old_value = q_table[state][action];
    double max_next_value = -INFINITY;
    // Calculate the maximum Q value in the new state
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (q_table[next_state][i] > max_next_value) {
            max_next_value = q_table[next_state][i];
        }
    }

    // Update Q value for given state and action
    q_table[state][action] = old_value + ALPHA * (reward + GAMMA * max_next_value - old_value);
}

// Function to calculate the state of the board
int calculate_state(Board *board) {
    int state = 0;
    int power = 1; // Start at 3^0
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            state += board->cells[i][j] * power;
            power *= BOARD_SIZE; // Increases the power of 3
        }
    }
    return (state);
}

// Function to update the board status
void update_board(Board *board, int action, Agent *agent) {
    // Calculate the row and column of the action
    int row = action / BOARD_SIZE;
    int col = action % BOARD_SIZE;

	board->cells[row][col] = agent->player;
	agent->player = (agent->player == X) ? O : X;
}

// Function to check if the game is over (winner)
bool is_win_state(Board *board) {
    // Check rows and columns
    for (int i = 0; i < BOARD_SIZE; i++) {
        if ((board->cells[i][0] == board->cells[i][1] && board->cells[i][1] == board->cells[i][2] && board->cells[i][0] != 0) ||
            (board->cells[0][i] == board->cells[1][i] && board->cells[1][i] == board->cells[2][i] && board->cells[0][i] != 0)) {
            return (true);
        }
    }

    // Check diagonals
    if ((board->cells[0][0] == board->cells[1][1] && board->cells[1][1] == board->cells[2][2] && board->cells[0][0] != 0) ||
        (board->cells[0][2] == board->cells[1][1] && board->cells[1][1] == board->cells[2][0] && board->cells[0][2] != 0)) {
        return (true);
    }

    // No win state
    return (false);
}

// Function to check if the game is over (draw)
bool is_draw_state(Board *board) {
    // Check if all cells are filled
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board->cells[i][j] == 0) {
                // If any cell is empty, it's not a draw state
                return (false);
            }
        }
    }

    // If all cells are filled and it's not a win state, it's a draw state
    if (!is_win_state(board)) {
        return (true);
    }

    // Otherwise, it's not a draw state
    return (false);
}

bool is_game_over(Board *board) {
	return (is_win_state(board) || is_draw_state(board));
}

double calculate_reward(Board *board, int player_turn) {
	if (is_win_state(board)) {
		if (player_turn == X) {
			return (REWARD_WIN); // Agent wins
		}
	} else if (is_draw_state(board)) {
		return (REWARD_DRAW); // Draw
	}
	return (REWARD_LOSE); // Agent losses
}

// Function to get user action
int get_user_action(Board *board) {
    int action;
    while (1) {
        printf("Enter your action (0-8): ");
        scanf("%d", &action);
        if (action < 0 || action > 8 || board->cells[action / BOARD_SIZE][action % BOARD_SIZE] != EMPTY) {
            printf("Invalid action. Please select an empty cell (0-8).\n");
        } else {
            break;
        }
    }
    return (action);
}

// Function to select an action based on the epsilon-greedy policy
int select_action(Agent *agent, Board *board, int state) {
    // Generate a random number between 0 and 1
    srand(time(NULL));
    double rand_num = (double)rand() / (double)RAND_MAX;

    if (rand_num < EPSILON) {
        // Exploration: Select a random action
        int action;
        int attempts = 0;  // Add a counter for attempts
        int empty_cells = 0;
        for (int i = 0; i < NUM_ACTIONS; ++i) {
            if (board->cells[i / BOARD_SIZE][i % BOARD_SIZE] == EMPTY) {
                empty_cells++;
            }
        }
        if (empty_cells == 0) {
            return (-1);  // No valid actions
        }
        do {
            action = rand() % NUM_ACTIONS;
            attempts++;  // Increment attempts counter
        } while (board->cells[action / BOARD_SIZE][action % BOARD_SIZE] != EMPTY && attempts < NUM_ACTIONS); // Make sure the cell is empty
        return (action);
    } else {
        // Exploitation: Select the best known action
        int best_action = -1;
        double best_q_value = -INFINITY;
        for (int action = 0; action < NUM_ACTIONS; ++action) {
            if (board->cells[action / BOARD_SIZE][action % BOARD_SIZE] == EMPTY && agent->q_table[state][action] > best_q_value) {
                best_action = action;
                best_q_value = agent->q_table[state][action];
            }
        }
        return (best_action);
    }
}

// Function to evaluate agent performance
void evaluate_agent(Board *board, Agent *agent) {
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int state;
    int new_state;
	int action;

    for (int i = 0; i < EPISODES; ++i) {
        initialize_board(board); // Reset the board for each game
        int game_over = 0;

        while (!game_over) {
        state = calculate_state(board);

        // Agent movement
		action = select_action(agent, board, state);
        update_board(board, action, agent);

        // Check if the game is over
        game_over = is_game_over(board);
        }

		// Check the result of the game
		agent->player = (agent->player == X) ? O : X;
		double reward = calculate_reward(board, agent->player);

		(reward == REWARD_WIN) ? wins++ : (reward == REWARD_DRAW) ? draws++ : losses++;

		new_state = calculate_state(board);
		update_q_table(agent->q_table, state, select_action(agent, board, new_state), reward, new_state);
    }

    // Print evaluation statistics
    printf("Wins: %d, Draws: %d, Losses: %d\n", wins, draws, losses);
}

// Save the q table in a .csv (after training)
void save_q_table_to_file(double q_table[NUM_STATES][NUM_ACTIONS], const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int state = 0; state < NUM_STATES; state++) {
        for (int action = 0; action < NUM_ACTIONS; action++) {
            fprintf(file, "%f", q_table[state][action]);
            if (action < NUM_ACTIONS - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Load any table to change the trained mode
void load_q_table_from_file(double q_table[NUM_STATES][NUM_ACTIONS], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int state = 0; state < NUM_STATES; state++) {
        for (int action = 0; action < NUM_ACTIONS; action++) {
            if (fscanf(file, "%lf,", &q_table[state][action]) != 1) {
                printf("Error reading file!\n");
                fclose(file);
                return;
            }
        }
    }

    fclose(file);
}

int main(void) {
    Board board;
    Agent agent;
    srand(time(NULL)); // Initialize the random number generator
    initialize_board(&board);
    initialize_agent(&agent);

    int state = calculate_state(&board);
    int turn = 0; // 0 for the agent, 1 for the user

    evaluate_agent(&board, &agent);

	// Show final table
	save_q_table_to_file(agent.q_table, "q_table.csv");

    // Load any table (uncomment to load the table)
    //load_q_table_from_file(agent.q_table, "q_table_example.csv");

    initialize_board(&board);
    while (1) {
        // Show board
        display_board(&board);

        int action;
        if (turn == 0) {
            // Allow the agent to select an action
            action = select_action(&agent, &board, state);
        } else {
            // Get user action
            action = get_user_action(&board);
        }

        // Update board status
        update_board(&board, action, &agent);

        // Calculate the reward
        double reward = calculate_reward(&board, turn == 0 ? X : O);

        // Calculate the new state
        int new_state = calculate_state(&board);

        // Update Q values if it is the agent's turn
        if (turn == 0) {
            update_q_table(agent.q_table, state, action, reward, new_state);
        }

        // Update status
        state = new_state;

        // Change shift
        turn = 1 - turn;

        // End the game if an end state is reached
        if (is_game_over(&board)) {
            if (is_win_state(&board)) {
                if (turn == 1) {
                    printf("El agente (IA) ha ganado el juego!\n");
                } else {
                    printf("El jugador ha ganado el juego!\n");
                }
            } else if (is_draw_state(&board)) {
                printf("El juego ha terminado en empate.\n");
            }
            break;
        }
    }
    return (0);
}
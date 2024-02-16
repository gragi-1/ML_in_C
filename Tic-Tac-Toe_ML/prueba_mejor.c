#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define BOARD_SIZE 3
#define EPISODES 10000
#define EPSILON 0.2
#define ALPHA 0.5
#define GAMMA 0.9
#define NUM_STATES 19683 // 3^9
#define NUM_ACTIONS 9
#define REWARD_WIN 1.0
#define REWARD_LOSE -1.0
#define REWARD_DRAW 0.5
#define REWARD_NONE 0.0
#define PLAYER_TURN 1
#define EMPTY 0
#define X 1
#define O 2

typedef struct {
    int cells[3][3]; // Representa el estado del tablero, donde 0 es una celda vacía, 1 es una 'X' y 2 es una 'O'
} Board;

typedef struct {
    double q_table[NUM_STATES][NUM_ACTIONS]; // Tabla Q
    int player; // Jugador actual
} Agent;

// Función para inicializar el agente y la tabla Q
void initialize_agent(Agent *agent) {
    for (int i = 0; i < NUM_STATES; ++i) {
        for (int j = 0; j < NUM_ACTIONS; ++j) {
            agent->q_table[i][j] = 0.0; // Inicializar todos los valores Q a 0
        }
    }
    agent->player = X; // El agente juega como 'X'
}

// Función para inicializar el tablero
void initialize_board(Board *board) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board->cells[i][j] = EMPTY;
        }
    }
}

// Función para mostrar el tablero
void display_board(Board *board) {
    printf("=====\n");
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
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

// Función para actualizar los valores Q
void update_q_table(double q_table[NUM_STATES][NUM_ACTIONS], int state, int action, int reward, int next_state) {
    double old_value = q_table[state][action];
    double max_next_value = -INFINITY;
    // Calcular el valor Q máximo en el nuevo estado
    for (int i = 0; i < NUM_ACTIONS; i++) {
        if (q_table[next_state][i] > max_next_value) {
            max_next_value = q_table[next_state][i];
        }
    }

    // Actualizar el valor Q para el estado y la acción dados
    q_table[state][action] = old_value + ALPHA * (reward + GAMMA * max_next_value - old_value);
}

// Función para calcular el estado del tablero
int calculate_state(Board *board) {
    int state = 0;
    int power = 1; // Comienza con 3^0
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            state += board->cells[i][j] * power;
            power *= 3; // Incrementa la potencia de 3
        }
    }
    return state;
}

// Función para calcular la recompensa
double calculate_reward(Board *board) {
    // Comprobar si hay un ganador
    for (int i = 0; i < 3; ++i) {
        if ((board->cells[i][0] == board->cells[i][1] && board->cells[i][1] == board->cells[i][2] && board->cells[i][0] != EMPTY) || // Comprobar las filas
            (board->cells[0][i] == board->cells[1][i] && board->cells[1][i] == board->cells[2][i] && board->cells[0][i] != EMPTY)) { // Comprobar las columnas
            return (board->cells[i][0] == X) ? REWARD_WIN : REWARD_LOSE;
        }
    }
    if ((board->cells[0][0] == board->cells[1][1] && board->cells[1][1] == board->cells[2][2] && board->cells[0][0] != EMPTY) || // Comprobar la diagonal principal
        (board->cells[0][2] == board->cells[1][1] && board->cells[1][1] == board->cells[2][0] && board->cells[0][2] != EMPTY)) { // Comprobar la diagonal secundaria
        return (board->cells[1][1] == X) ? REWARD_WIN : REWARD_LOSE;
    }

    // Comprobar si el tablero está lleno (empate)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (!(board->cells[i][j] == EMPTY)) {
                return REWARD_NONE; // El juego aún no ha terminado
            }
        }
    }

    // Si no hay un ganador y el tablero está lleno, el juego es un empate
    return REWARD_DRAW;
}

int player_turn = 1;

// Función para actualizar el estado del tablero
void update_board(Board *board, int action) {
    // Calcular la fila y la columna de la acción
    int row = action / 3;
    int col = action % 3;

    // Actualizar la celda correspondiente en el tablero
    if (player_turn == 1) {
        board->cells[row][col] = X;
        player_turn = 2;
    } else {
        board->cells[row][col] = O;
        player_turn = 1;
    }
}

// Función para verificar si el juego ha terminado (ganador)
int is_win_state(Board *board) {
    // Check rows and columns
    for (int i = 0; i < 3; i++) {
        if ((board->cells[i][0] == board->cells[i][1] && board->cells[i][1] == board->cells[i][2] && board->cells[i][0] != 0) ||
            (board->cells[0][i] == board->cells[1][i] && board->cells[1][i] == board->cells[2][i] && board->cells[0][i] != 0)) {
            return 1;
        }
    }

    // Check diagonals
    if ((board->cells[0][0] == board->cells[1][1] && board->cells[1][1] == board->cells[2][2] && board->cells[0][0] != 0) ||
        (board->cells[0][2] == board->cells[1][1] && board->cells[1][1] == board->cells[2][0] && board->cells[0][2] != 0)) {
        return 1;
    }

    // No win state
    return 0;
}

// Función para verificar si el juego ha terminado (empate)
int is_draw_state(Board *board) {
    // Check if all cells are filled
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board->cells[i][j] == 0) {
                // If any cell is empty, it's not a draw state
                return 0;
            }
        }
    }

    // If all cells are filled and it's not a win state, it's a draw state
    if (!is_win_state(board)) {
        return 1;
    }

    // Otherwise, it's not a draw state
    return 0;
}

int is_game_over(Board *board) {

	if (is_win_state(board) == 1) {
		return 1;
	} else if (is_draw_state(board) == 1) {
		return -1;
	} else {
		return 0;
	}
}

// Función para obtener la acción del usuario
int get_user_action(Board *board) {
    int action;
    while (1) {
        printf("Enter your action (0-8): ");
        scanf("%d", &action);
        if (action < 0 || action > 8 || board->cells[action / 3][action % 3] != EMPTY) {
            printf("Invalid action. Please select an empty cell (0-8).\n");
        } else {
            break;
        }
    }
    return action;
}

// Función para ejecutar una acción (el agente selecciona una acción)
void execute_action(Board *board, int action, int player) {
    // Calculate the row and column from the action
    int row = action / BOARD_SIZE;
    int col = action % BOARD_SIZE;

    // Update the cell with the player's symbol
    board->cells[row][col] = player;
}

// Convert the board to a state
char* get_state(Board *board) {
    char* state = malloc((BOARD_SIZE * BOARD_SIZE + 1) * sizeof(char));
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            switch (board->cells[i][j]) {
                case EMPTY:
                    state[i * BOARD_SIZE + j] = '0';
                    break;
                case X:
                    state[i * BOARD_SIZE + j] = '1';
                    break;
                case O:
                    state[i * BOARD_SIZE + j] = '2';
                    break;
            }
        }
    }
    state[BOARD_SIZE * BOARD_SIZE] = '\0';  // Null-terminate the string
    return state;
}

// Función para seleccionar una acción basada en la política epsilon-greedy
int select_action(Agent *agent, Board *board, int state) {
    // Generar un número aleatorio entre 0 y 1
    srand(time(NULL));
    double rand_num = (double)rand() / (double)RAND_MAX;

    if (rand_num < EPSILON) {
        // Exploración: seleccionar una acción aleatoria
        int action;
        int attempts = 0;  // Add a counter for attempts
        int empty_cells = 0;
        for (int i = 0; i < NUM_ACTIONS; ++i) {
            if (board->cells[i / 3][i % 3] == EMPTY) {
                empty_cells++;
            }
        }
        if (empty_cells == 0) {
            return -1;  // No valid actions
        }
        do {
            action = rand() % NUM_ACTIONS;
            attempts++;  // Increment attempts counter
        } while (board->cells[action / 3][action % 3] != EMPTY && attempts < NUM_ACTIONS); // Asegurarse de que la celda esté vacía
        return action;
    } else {
        // Explotación: seleccionar la mejor acción conocida
        int best_action = -1;
        double best_q_value = -INFINITY;
        for (int action = 0; action < NUM_ACTIONS; ++action) {
            if (board->cells[action / 3][action % 3] == EMPTY && agent->q_table[state][action] > best_q_value) {
                best_action = action;
                best_q_value = agent->q_table[state][action];
            }
        }
        return best_action;
    }
}

void make_random_move(Board *board, char current_player) {
    int empty_cells[9];
    int num_empty_cells = 0;

    // Encuentra todas las celdas vacías
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board->cells[i][j] == EMPTY) {
                empty_cells[num_empty_cells] = i * 3 + j; // Convertir la posición 2D a una posición 1D
                num_empty_cells++;
            }
        }
    }

    if (num_empty_cells > 0) {
        // Selecciona una celda vacía al azar
        int random_index = rand() % num_empty_cells;
        int random_cell = empty_cells[random_index];

        // Convertir la posición 1D a una posición 2D
        int row = random_cell / 3;
        int col = random_cell % 3;

        // Coloca la ficha del jugador actual en la celda seleccionada
        board->cells[row][col] = current_player;
    }
}

// Función para evaluar el desempeño del agente
void evaluate_agent(Board *board, Agent *agent) {
    int wins = 0;
    int draws = 0;
    int losses = 0;
    int state;
    int new_state;

    for (int i = 0; i < EPISODES; ++i) {
        initialize_board(board); // Reiniciar el tablero para cada juego

        int current_player = 1; // El agente juega como 'X'
        int game_over = 0;

        while (!game_over) {
        state = calculate_state(board);

        // Movimiento del agente
        execute_action(board, select_action(agent, board, state), agent->player);
        
        update_board(board, select_action(agent, board, state));

        new_state = calculate_state(board);

        update_q_table(agent->q_table, state, select_action(agent, board, new_state), calculate_reward(board), new_state);

        // Cambiar el turno
        current_player = 1 - current_player;

        (current_player == 1) ? (agent->player = X) : (agent->player = O);

        // Imprime el tablero después de cada movimiento para depuración
        //display_board(board);

        // Verifica si el juego ha terminado
        game_over = is_game_over(board);

        // Imprime el estado del juego después de cada movimiento para depuración
        //printf("Game over: %d\n", game_over);
        }

        // Actualiza las estadísticas según el resultado del juego
        switch (game_over) {
            case 1: // Victoria del agente
                wins++;
                break;
            case -1: // Empate
                draws++;
                break;
            case 0: // Derrota del agente
                losses++;
                break;
        }
    }

    // Imprime las estadísticas de la evaluación
    printf("Wins: %d, Draws: %d, Losses: %d\n", wins, draws, losses);
}

void print_q_values(double q_table[NUM_STATES][NUM_ACTIONS]) {
    for (int state = 0; state < NUM_STATES; ++state) {
        printf("State %d:\n", state);
        for (int action = 0; action < NUM_ACTIONS; ++action) {
            printf("  Action %d: %f\n", action, q_table[state][action]);
        }
    }
}

int main() {
    Board board;
    Agent agent;
    srand(time(NULL)); // Inicializa el generador de números aleatorios
    initialize_board(&board);
    initialize_agent(&agent);

    int state = calculate_state(&board);
    int turn = 0; // 0 para el agente, 1 para el usuario

    evaluate_agent(&board, &agent);

    initialize_board(&board);
    while (1) {
        // Mostrar el tablero
        display_board(&board);

        int action;
        if (turn == 0) {
            // Permitir que el agente seleccione una acción
            action = select_action(&agent, &board, state);
        } else {
            // Obtener la acción del usuario
            action = get_user_action(&board);
        }

        // Actualizar el estado del tablero
        update_board(&board, action);

        // Calcular la recompensa
        double reward = calculate_reward(&board);

        // Calcular el nuevo estado
        int new_state = calculate_state(&board);

        // Actualizar los valores Q si es el turno del agente
        if (turn == 0) {
            update_q_table(agent.q_table, state, action, reward, new_state);
        }

        // Actualizar el estado
        state = new_state;

        // Cambiar el turno
        turn = 1 - turn;

        // Terminar el juego si se alcanza un estado final
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

    // Mostrar q tabla final
    print_q_values(agent.q_table);

    return 0;
}
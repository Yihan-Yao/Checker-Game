/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Yihan Yao
  Dated:     2021/9/16

*/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE             8          // board size
#define ROWS_WITH_PIECES       3          // number of initial rows with pieces
#define CELL_EMPTY             '.'        // empty cell character
#define CELL_BPIECE            'b'        // black piece character
#define CELL_WPIECE            'w'        // white piece character
#define CELL_BTOWER            'B'        // black tower character
#define CELL_WTOWER            'W'        // white tower character
#define COST_PIECE             1          // one piece cost
#define COST_TOWER             3          // one tower cost
#define TREE_DEPTH             3          // minimax tree depth
#define COMP_ACTIONS           10         // number of computed actions
#define ILLIGAL_ACTION         -1         // code for illigal action
#define PIECE_BUFFER_SIZE      4          // buffer size for a piece's moves
#define TRUE                   1          // int value for boolean true
#define FALSE                  0          // int value for boolean false
#define BOARD_BUFFER_SIZE      48         // buffer size for all moves possible
#define PLAYER_WHITE_TAG       "WHITE"    // player tag for white
#define PLAYER_BLACK_TAG       "BLACK"    // player tag for black
#define PLAYER_TAG_LENGTH      6          // player tag length
#define INPUT_COMMAND_LENGTH   6          // input command length

/* represents a move */
struct Move {
    int source;
    int target;
};

/* function prototypes */
char* setup_board();
int stage_0(char* board, char* current_player, char* last_command_pointer);
void stage_1(char* board, char* current_player, int move_count); 
void print_board(char* board);
void read_move(char input[], int* source_pointer, int* targeSSt_pointer);
char* move(char* board, int source, int target);
int calculate_board_cost(char* board);
int command_to_set(char column, int row);
char* set_to_command(int source, int target);
void validify_move(char* board, int source, int target, char* current_player);
int* find_targets_piece(char* board, int source);
int find_moves_board(char* board, char* current_player, struct Move* moves);
int minimax(char* board, int depth, char* current_player);
void minimax_root(char* board, int depth, char* current_player, struct Move* best_move_pointer);

int
main(int argc, char *argv[]) {
    // setup board
    char* board = setup_board();
    char* current_player = malloc(PLAYER_TAG_LENGTH * sizeof(char));
    strcpy(current_player, PLAYER_BLACK_TAG);
    char last_command = 0;
    char* last_command_pointer = &last_command;
    int move_count = stage_0(board, current_player, last_command_pointer);  // stage 0
    if(last_command == 'A') {
        stage_1(board, current_player, move_count);                         // stage 1
    }
    
    // free the memory
    free(board);
    free(current_player);

    return EXIT_SUCCESS;             // exit program with the success code
}

int
stage_0(char* board, char* current_player, char* last_command) {
    // print first board
    printf("BOARD SIZE: 8x8\n");
    printf("#BLACK PIECES: 12\n");
    printf("#WHITE PIECES: 12\n");
    print_board(board);

    int move_count = 1;
    int source = ILLIGAL_ACTION;
    int target = ILLIGAL_ACTION;
    int* source_pointer = &source;
    int* target_pointer = &target;
    char input[INPUT_COMMAND_LENGTH];

    // scan each action from stdin, perform action if allowed, then print the board
    while(scanf("%s\n",input) != EOF) {
        // if input is 'A', jump to stage 1
        if(strcmp(input, "A")==0) {
            *last_command = 'A';
            return move_count;
        // if input is 'P', jump to stage 2
        } else if(strcmp(input, "P")==0) {
            *last_command = 'P';
            return move_count;
        }

        // acquire souce and target index from input
        read_move(input, source_pointer, target_pointer);

        // check for errors
        validify_move(board, source, target, current_player);

        // print header
        printf("=====================================\n");
        printf("%s ACTION #%d: %s\n", current_player, move_count++, input);

        // make the move on the board
        board = move(board, source, target);

        // print current board cost
        printf("BOARD COST: %d\n", calculate_board_cost(board));

        // print the board
        print_board(board);

        // change player
        if(strcmp(current_player, PLAYER_BLACK_TAG)==0) {
            strcpy(current_player, PLAYER_WHITE_TAG);
        } else {
            strcpy(current_player, PLAYER_BLACK_TAG);
        }
    }
    return move_count;
}

void
stage_1(char* board, char* current_player, int move_count) {
    // set depth to default value
    int depth = TREE_DEPTH;

    // if cost is INT_MAX or INT_MIN which means the game ends
    // print corresponding message
    int cost = calculate_board_cost(board);
    if(cost==INT_MAX) {
        printf("BLACK WIN!\n");
    } else if(cost==INT_MIN) {
        printf("WHITE WIN!\n");
    }

    // get the best move using the "minimax decision rule"
    struct Move best_move;
    best_move.source = -1;
    best_move.target = -1;

    struct Move* best_move_pointer = &best_move;
    minimax_root(board, depth, current_player, best_move_pointer);

    // print header
    printf("=====================================\n");
    printf("*** %s ACTION #%d: ", current_player, move_count++);

    // translate the move to a command, and print the move command
    printf("%s\n", set_to_command(best_move.source, best_move.target));

    // make the move on the board
    board = move(board, best_move.source, best_move.target);
    
    // print current board cost
    printf("BOARD COST: %d\n", calculate_board_cost(board));

    // print the board
    print_board(board);

    // change player just in case
    if(strcmp(current_player, PLAYER_BLACK_TAG)==0) {
        strcpy(current_player, PLAYER_WHITE_TAG);
    } else {
        strcpy(current_player, PLAYER_BLACK_TAG);
    }
}

/* 
 * root function for the "minimax decision rule" strategy to kick off the recuision
 * returns: the best move of the type (struct Move)
 */
void 
minimax_root(char* board, int depth, char* current_player, struct Move* best_move_pointer) {
    int best_cost;          // to record the current best cost for current player

    // if looking for best move for black
    if(strcmp(current_player, PLAYER_BLACK_TAG)==0) {
        // set the best cost to worst case first
        best_cost = INT_MIN;

        // aquire all possible moves for current player
        struct Move* moves = malloc(BOARD_BUFFER_SIZE * sizeof(struct Move));
        int number_of_moves = find_moves_board(board, current_player, moves);

        // find out the best cost for all child nodes
        char* child_board = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));
        int i;
        for(i=0; i<number_of_moves; i++) {
            // make a copy of original board
            memcpy(child_board, board, BOARD_SIZE*BOARD_SIZE);
            // make the move
            child_board = move(child_board, moves[i].source, moves[i].target);
            // calculate new cost
            int evaluation = minimax(child_board, depth-1, PLAYER_WHITE_TAG);
            // higher is better for black
            if(evaluation > best_cost) {
                best_cost = evaluation;
                // record the best move so far
                best_move_pointer->source = moves[i].source;
                best_move_pointer->target = moves[i].target;
            }
        }
        free(child_board);
        free(moves);
    // if looking for best move for white
    } else {
        // set the best cost to worst case first
        best_cost = INT_MAX;

        // aquire all possible moves for current player
        struct Move* moves = malloc(BOARD_BUFFER_SIZE * sizeof(struct Move));
        int number_of_moves = find_moves_board(board, current_player, moves);

        // find out the best cost for all child nodes
        char* child_board = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));
        int i;
        for(i=0; i<number_of_moves; i++) {
            // make a copy of original board
            memcpy(child_board, board, BOARD_SIZE*BOARD_SIZE);
            // make the move
            child_board = move(child_board, moves[i].source, moves[i].target);
            // calculate new cost
            int evaluation = minimax(child_board, depth-1, PLAYER_BLACK_TAG);
            // lower is better for white
            if(evaluation < best_cost) {
                best_cost = evaluation;
                // record the best move so far
                best_move_pointer->source = moves[i].source;
                best_move_pointer->target = moves[i].target;
            }
        }
        free(child_board);
        free(moves);
    }
}

/* 
 * recuisive function for the "minimax decision rule" strategy
 * returns: cost of best choice for current player for current board position
 */
int 
minimax(char* board, int depth, char* current_player) {
    // if cost is INT_MAX or INT_MIN which means the game ends
    // or if depth is reached, return cost
    int cost = calculate_board_cost(board);
    if(depth==0 || cost==INT_MAX || cost==INT_MIN) {
        return cost;
    }
    int best_cost;          // to record the current best cost for current player

    // if looking for best move for black
    if(strcmp(current_player, PLAYER_BLACK_TAG)==0) {
        // set the best cost to worst case first
        best_cost = INT_MIN;

        // aquire all possible moves for current player
        struct Move* moves = malloc(BOARD_BUFFER_SIZE * sizeof(struct Move));
        int number_of_moves = find_moves_board(board, current_player, moves);

        // find out the best cost for all child nodes
        char* child_board = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));
        int i;
        for(i=0; i<number_of_moves; i++) {
            // make a copy of original board
            memcpy(child_board, board, BOARD_SIZE*BOARD_SIZE);
            // make the move
            child_board = move(child_board, moves[i].source, moves[i].target);
            // calculate new cost
            int evaluation = minimax(child_board, depth-1, PLAYER_WHITE_TAG);
            // higher is better for black
            if(evaluation > best_cost) {
                best_cost = evaluation;
            }
        }
        free(child_board);
        free(moves);
    // if looking for best move for white
    } else {
        // set the best cost to worst case first
        best_cost = INT_MAX;

        // aquire all possible moves for current player
        struct Move* moves = malloc(BOARD_BUFFER_SIZE * sizeof(struct Move));
        int number_of_moves = find_moves_board(board, current_player, moves);

        // find out the best cost for all child nodes
        char* child_board = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));
        int i;
        for(i=0; i<number_of_moves; i++) {
            // make a copy of original board
            memcpy(child_board, board, BOARD_SIZE*BOARD_SIZE);
            // make the move
            child_board = move(child_board, moves[i].source, moves[i].target);
            // calculate new cost
            int evaluation = minimax(child_board, depth-1, PLAYER_BLACK_TAG);
            // lower is better for white
            if(evaluation < best_cost) {
                best_cost = evaluation;
            }
        }
        free(child_board);
        free(moves);
    }
    return best_cost;
}

/* 
 * print the current board
 */
void 
print_board(char* board) {
    printf("     A   B   C   D   E   F   G   H\n");
    printf("   +---+---+---+---+---+---+---+---+\n");
    int i, j;
    for(i=0; i<BOARD_SIZE; i++) {
        printf(" %d |", i+1);
        for(j=0; j<BOARD_SIZE; j++) {
            printf(" %c |", board[i*BOARD_SIZE+j]);
        }
        printf("\n   +---+---+---+---+---+---+---+---+\n");
    }
}

/* 
 * create a board, initialise pieces, and return the pointer
 * returns: the pointer to the board array
 * caution: need to free memory outside of this function
 */
char* 
setup_board() {
    char* board = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));
    int i, j;
    for(i=0; i<BOARD_SIZE; i++) {
        for(j=0; j<BOARD_SIZE; j++) {
            if(i==0 || i==2) {  // 1st and 3rd row
                if(j%2==0) {
                    board[i*BOARD_SIZE+j] = CELL_EMPTY;
                } else {
                    board[i*BOARD_SIZE+j] = CELL_WPIECE;
                }
            } else if(i==1) {  // 2nd row
                if(j%2==0) {
                    board[i*BOARD_SIZE+j] = CELL_WPIECE;
                } else {
                    board[i*BOARD_SIZE+j] = CELL_EMPTY;
                }
            } else if(i==5 || i==7) {  // 6th and 8th row
                if(j%2==0) {
                    board[i*BOARD_SIZE+j] = CELL_BPIECE;
                } else {
                    board[i*BOARD_SIZE+j] = CELL_EMPTY;
                }
            } else if(i==6) {  // 7th row
                if(j%2==0) {
                    board[i*BOARD_SIZE+j] = CELL_EMPTY;
                } else {
                    board[i*BOARD_SIZE+j] = CELL_BPIECE;
                }
            } else {
                board[i*BOARD_SIZE+j] = CELL_EMPTY;
            }
        }
    }
    return board;
}

/* 
 * read in a move from stdin, print the move,
 * translate command to index, output through pointers
 */
void
read_move(char input[], int* source_pointer, int* target_pointer) {
    // read in a move
    char column_1, column_2;
    int row_1, row_2;
    sscanf(input, "%c%d-%c%d", &column_1, &row_1, &column_2, &row_2);

    // translate the move to cell index in board set and output through pointers
    *source_pointer = command_to_set(column_1,row_1);
    *target_pointer = command_to_set(column_2,row_2);
}

/* 
 * calculate the cost of the given board
 * returns: cost of given board
 *          INT_MIN if white wins, INT_MAX if black wins
 */
int
calculate_board_cost(char* board) {
    struct Move* white_moves = malloc(BOARD_BUFFER_SIZE * sizeof(struct Move));
    // if white has no moves, black wins
    if(find_moves_board(board, PLAYER_WHITE_TAG, white_moves) == 0) {
        return INT_MAX;
    }
    free(white_moves);

    struct Move* black_moves = malloc(BOARD_BUFFER_SIZE * sizeof(struct Move));
    // if black has no moves, white wins
    if(find_moves_board(board, PLAYER_BLACK_TAG, black_moves) == 0) {
        return INT_MIN;
    }
    free(black_moves);

    // sum all piece values to get cost
    int cost = 0;
    int i;
    for(i=0; i<(BOARD_SIZE*BOARD_SIZE); i++) {
        switch(board[i]) {
            case CELL_BPIECE:
                cost+=1;
                break;
            case CELL_BTOWER:
                cost+=3;
                break;
            case CELL_WPIECE:
                cost-=1;
                break;
            case CELL_WTOWER:
                cost-=3;
                break;
        }
    }
    return cost;
}

/* 
 * reads command from input and make the move on the board
 */
char*
move(char* board, int source, int target) {
    char* new_board = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));
    memcpy(new_board, board, BOARD_SIZE * BOARD_SIZE * sizeof(char));
    free(board);

    if(source<0 || source>=(BOARD_SIZE*BOARD_SIZE) ||
    target<0 || target>=(BOARD_SIZE*BOARD_SIZE)) {
        exit(EXIT_FAILURE);
    }

    // if captures remove the captured piece
    if(target==(source+18)) {
        new_board[source+9] = CELL_EMPTY;
    } else if(target==(source+14)) {
        new_board[source+7] = CELL_EMPTY;
    } else if(target==(source-18)) {
        new_board[source-9] = CELL_EMPTY;
    } else if(target==(source-14)) {
        new_board[source-7] = CELL_EMPTY;
    }
    // if opposide side is reached promote to tower
    if(target>=command_to_set('A',8) && target<=command_to_set('H',8)
        && new_board[source] == CELL_WPIECE) {
            new_board[source] = CELL_WTOWER;
    } else if(target>=command_to_set('A',1) && target<=command_to_set('H',1)
        && new_board[source] == CELL_BPIECE) {
            new_board[source] = CELL_BTOWER;
    }
    // moves the piece
    new_board[target] = new_board[source];
    new_board[source] = CELL_EMPTY;

    return new_board;
}

/* 
 * check if the move from source to target is allowed
 * print error message and exit program if an error occurs
 */
void
validify_move(char* board, int source, int target, char* current_player) {
    // check for ERROR 1
    if(source == ILLIGAL_ACTION) {
        fprintf(stderr, "ERROR: Source cell is outside of the board.\n");
        exit(EXIT_FAILURE);
    // check for ERROR 2
    } else if(target == ILLIGAL_ACTION) {
        fprintf(stderr, "ERROR: Target cell is outside of the board.\n");
        exit(EXIT_FAILURE);
    // check for ERROR 3
    } else if(board[source] == CELL_EMPTY) {
        fprintf(stderr, "ERROR: Source cell is empty.\n");
        exit(EXIT_FAILURE);
    // check for ERROR 4
    } else if(board[target] != CELL_EMPTY) {
        fprintf(stderr, "ERROR: Target cell is not empty.\n");
        exit(EXIT_FAILURE);
    // check for ERROR 5
    } else if(strcmp(current_player, PLAYER_WHITE_TAG)==0) {
        if(board[source] == CELL_BPIECE || board[source] == CELL_BTOWER) {
            fprintf(stderr, "ERROR: Source cell holds opponent's piece/tower.\n");
            exit(EXIT_FAILURE);
        }
    } else if(strcmp(current_player, PLAYER_BLACK_TAG)==0) {
        if(board[source] == CELL_WPIECE || board[source] == CELL_WTOWER) {
            fprintf(stderr, "ERROR: Source cell holds opponent's piece/tower.\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // find all legal targets
    int* legal_targets = find_targets_piece(board, source);
    
    int i, n=0;
    // check if target is any of the legal targets
    for(i=0; i<PIECE_BUFFER_SIZE; i++) {
        if(target == legal_targets[i]) {
            n++;
        }
    }
    // if target is not a legal target, raise ERROR 6
    if(n==0) {
        fprintf(stderr, "ERROR: Illegal action.\n");
        exit(EXIT_FAILURE);
    }

    // free memory from function find_targets_piece
    free(legal_targets);
}

/* 
 * find all possible targets to move to from the source cell
 * returns: all possible targets' set numbers in an array of size PIECE_BUFFER_SIZE
 *          returns all ILLIGAL_ACTION if no moves are possible from the source cell
 * caution: need to free memory outside of this function
 */
int*
find_targets_piece(char* board, int source) {
    // create an array to store all possible moves' targets' set numbers
    int* targets = malloc(PIECE_BUFFER_SIZE * sizeof(int));
    // initialize array to store error code for out of board "ILLIGAL_ACTION"
    int i;
    for(i=0; i<PIECE_BUFFER_SIZE; i++) {
        targets[i] = ILLIGAL_ACTION;
    }

    // explore for possible moves from north-east direction clockwisely
    // if source is a white piece
    if(board[source]==CELL_WPIECE) {
        // if the square down and right is on the board
        if(source%8<7 && (source+9)<(BOARD_SIZE*BOARD_SIZE)) {
            // and empty
            if(board[source+9]==CELL_EMPTY) {
                targets[1] = source+9;  // add target to list
            }
            // or is a black piece
            // and the square down and right 2 squares is on the board and is empty
            else if((board[source+9]==CELL_BPIECE || board[source+9]==CELL_BTOWER)
                    && (source%8<6 && (source+18)<(BOARD_SIZE*BOARD_SIZE) && board[source+18]==CELL_EMPTY)) {
                targets[1] = source+18;  // add target to list
            }
        }
        // if the square down and left is on the board
        if(source%8>0 && (source+7)<(BOARD_SIZE*BOARD_SIZE)) {
            // and empty
            if(board[source+7]==CELL_EMPTY) {
                targets[2] = source+7;  // add target to list
            }
            // or is a black piece
            // and the square down and left 2 squares is on the board and is empty
            else if((board[source+7]==CELL_BPIECE || board[source+7]==CELL_BTOWER)
                    && (source%8>1 && (source+14)<(BOARD_SIZE*BOARD_SIZE) && board[source+14]==CELL_EMPTY)) {
                targets[2] = source+14;  // add target to list
            }
        }
    }
    // if source is a black piece
    else if(board[source]==CELL_BPIECE) {
        // if the square up and right is on the board
        if(source%8<7 && (source-7)>0) {
            // and empty
            if(board[source-7]==CELL_EMPTY) {
                targets[0] = source-7;  // add target to list
            }
            // or is a white piece
            // and the square up and right 2 squares is on the board
            else if((board[source-7]==CELL_WPIECE || board[source-7]==CELL_WTOWER)
                    && (source%8<6 && (source-14)>0 && board[source-14]==CELL_EMPTY)) {
                targets[0] = source-14;  // add target to list
            }
        }
        // if the square up and left is on the board
        if(source%8>0 && (source-9)>0) {
            // and empty
            if(board[source-9]==CELL_EMPTY) {
                targets[3] = source-9;  // add target to list
            }
            // or is a white piece
            // and the square up and left 2 squares is on the board and is empty
            else if((board[source-9]==CELL_WPIECE || board[source-9]==CELL_WTOWER)
                    && (source%8>1 && (source-18)>0 && board[source-18]==CELL_EMPTY)) {
                targets[3] = source-18;  // add target to list
            }
        }
    }
    // if source is a white tower
    else if(board[source]==CELL_WTOWER) {
        // if the square up and right is on the board
        if(source%8<7 && (source-7)>0) {
            // and empty
            if(board[source-7]==CELL_EMPTY) {
                targets[0] = source-7;  // add target to list
            }
            // or is a black piece 
            // and the square up and right 2 squares is on the board and is empty
            else if((board[source-7]==CELL_BPIECE || board[source-7]==CELL_BTOWER)
                    && (source%8<6 && (source-14)>0 && board[source-14]==CELL_EMPTY)) {
                targets[0] = source-14;  // add target to list
            }
        }
        // if the square down and right is on the board
        if(source%8<7 && (source+9)<(BOARD_SIZE*BOARD_SIZE)) {
            // and empty
            if(board[source+9]==CELL_EMPTY) {
                targets[1] = source+9;  // add target to list
            }
            // or is a black piece
            // and the square down and right 2 squares is on the board and is empty
            else if((board[source+9]==CELL_BPIECE || board[source+9]==CELL_BTOWER)
                    && (source%8<6 && (source+18)<(BOARD_SIZE*BOARD_SIZE) && board[source+18]==CELL_EMPTY)) {
                targets[1] = source+18;  // add target to list
            }
        }
        // if the square down and left is on the board
        if(source%8>0 && (source+7)<(BOARD_SIZE*BOARD_SIZE)) {
            // and empty
            if(board[source+7]==CELL_EMPTY) {
                targets[2] = source+7;  // add target to list
            }
            // or is a black piece
            // and the square down and left 2 squares is on the board and is empty
            else if((board[source+7]==CELL_BPIECE || board[source+7]==CELL_BTOWER)
                    && (source%8>1 && (source+14)<(BOARD_SIZE*BOARD_SIZE) && board[source+14]==CELL_EMPTY)) {
                targets[2] = source+14;  // add target to list
            }
        }
        // if the square up and left is on the board
        if(source%8>0 && (source-9)>0) {
            // and empty
            if(board[source-9]==CELL_EMPTY) {
                targets[3] = source-9;  // add target to list
            }
            // or is a black piece
            // and the square up and left 2 squares is on the board and is empty
            else if((board[source-9]==CELL_BPIECE || board[source-9]==CELL_BTOWER)
                    && (source%8>1 && (source-18)>0 && board[source-18]==CELL_EMPTY)) {
                targets[3] = source-18;  // add target to list
            }
        }
    }
    // if source is a black tower
    else if(board[source]==CELL_BTOWER) {
        // if the square up and right is on the board
        if(source%8<7 && (source-7)>0) {
            // and empty
            if(board[source-7]==CELL_EMPTY) {
                targets[0] = source-7;  // add target to list
            }
            // or is a white piece
            // and the square up and right 2 squares is on the board
            else if((board[source-7]==CELL_WPIECE || board[source-7]==CELL_WTOWER)
                    && (source%8<6 && (source-14)>0 && board[source-14]==CELL_EMPTY)) {
                targets[0] = source-14;  // add target to list
            }
        }
        // if the square down and right is on the board
        if(source%8<7 && (source+9)>0) {
            // and empty
            if(board[source+9]==CELL_EMPTY) {
                targets[1] = source+9;  // add target to list
            }
            // or is a piece of another color
            // and the square down and right 2 squares is on the board and is empty
            else if((board[source+9]==CELL_WPIECE || board[source+9]==CELL_WTOWER)
                    && (source%8<6 && (source+18)>0 && board[source+18]==CELL_EMPTY)) {
                targets[1] = source+18;  // add target to list
            }
        }
        // if the square down and left is on the board
        if(source%8>0 && (source+7)>0) {
            // and empty
            if(board[source+7]==CELL_EMPTY) {
                targets[2] = source+7;  // add target to list
            }
            // or is a piece of another color
            // and the square down and left 2 squares is on the board and is empty
            else if((board[source+7]==CELL_WPIECE || board[source+7]==CELL_WTOWER)
                    && (source%8>1 && (source+14)>0 && board[source+14]==CELL_EMPTY)) {
                targets[2] = source+14;  // add target to list
            }
        }
        // if the square up and left is on the board
        if(source%8>0 && (source-9)>0) {
            // and empty
            if(board[source-9]==CELL_EMPTY) {
                targets[3] = source-9;  // add target to list
            }
            // or is a white piece
            // and the square up and left 2 squares is on the board and is empty
            else if((board[source-9]==CELL_WPIECE || board[source-9]==CELL_WTOWER)
                    && (source%8>1 && (source-18)>0 && board[source-18]==CELL_EMPTY)) {
                targets[3] = source-18;  // add target to list
            }
        }
    }
    return targets;
}

/* 
 * find all legal actions for the current player on current board
 * returns: the total number of legal moves
 * caution: need to free memory outside of this function
 */
int
find_moves_board(char* board, char* current_player, struct Move* moves) {
    // buffer to store all possible moves
    int buffer_size = BOARD_BUFFER_SIZE;
    int move_count = 0;

    // find all legal moves for current player and put them into moves
    int i;
    int source;
    for(source=0; source<(BOARD_SIZE*BOARD_SIZE); source++) {
        if(strcmp(current_player, PLAYER_WHITE_TAG)==0 && (board[source]==CELL_WPIECE
                                               || board[source]==CELL_WTOWER)) {
            int* legal_targets = find_targets_piece(board, source);
            for(i=0; i<PIECE_BUFFER_SIZE; i++) {
                // put legal moves into moves
                if(legal_targets[i] != ILLIGAL_ACTION) {
                    struct Move move;
                    move.source = source;
                    move.target = legal_targets[i];
                    moves[move_count++] = move;
                }
            }
            // free memory from function find_targets_piece
            free(legal_targets);
        } else if(strcmp(current_player, PLAYER_BLACK_TAG)==0 && (board[source]==CELL_BPIECE
                                                      || board[source]==CELL_BTOWER)){
            int* legal_targets = find_targets_piece(board, source);
            for(i=0; i<PIECE_BUFFER_SIZE; i++) {
                // if buffer is full
                if((move_count+1) >= buffer_size) {
                    // expand buffer
                    buffer_size *= 2;
                    moves = realloc(moves, buffer_size * sizeof(struct Move));
                }
                // put legal moves into moves
                if(legal_targets[i] != ILLIGAL_ACTION) {
                    struct Move move;
                    move.source = source;
                    move.target = legal_targets[i];
                    moves[move_count++] = move;
                }
            }
            // free memory from function find_targets_piece
            free(legal_targets);
        }
    }
    return move_count;
}

/* 
 * converts a grid command to set index
 * returns: cell index in the board set
 *          returns ILLIGAL_ACTION if out of bounds 
 */
int 
command_to_set(char column, int row) {
    // return ILLIGAL_ACTION if the cell is outside of the board
    if(column < 'A' || column > 'H' || row < 1 || row > BOARD_SIZE) {
        return ILLIGAL_ACTION;
    }

    int column_int;
    switch(column) {
        case 'A':
            column_int = 1;
            break;
        case 'B':
            column_int = 2;
            break;
        case 'C':
            column_int = 3;
            break;
        case 'D':
            column_int = 4;
            break;
        case 'E':
            column_int = 5;
            break;
        case 'F':
            column_int = 6;
            break;
        case 'G':
            column_int = 7;
            break;
        case 'H':
            column_int = 8;
            break;
    }

    int cell_index = (row - 1) * 8 + column_int - 1;
    return cell_index;
}

/* 
 * converts set index to command
 * returns: a string containing a move command
 *          returns NULL if the source or target is outside of the board
 * caution: need to free memory outside of this function
 */
char* 
set_to_command(int source, int target) {
    // return NULL if the source or target is outside of the board
    if(source < 0 || target < 0 || source >= (BOARD_SIZE*BOARD_SIZE)
                                || target >= (BOARD_SIZE*BOARD_SIZE)) {
        return NULL;
    }

    char* command = malloc(INPUT_COMMAND_LENGTH * sizeof(char));
    command[0] = 'A' + (source % 8);
    command[1] = '1' + (source / 8);
    command[2] = '-';
    command[3] = 'A' + (target % 8);
    command[4] = '1' + (target / 8);

    return command;
}

/* THE END -------------------------------------------------------------------*/
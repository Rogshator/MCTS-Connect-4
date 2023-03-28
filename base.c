#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6
#define MAX_GAME_LENGHT 42

typedef int board[BOARD_WIDTH][BOARD_HEIGHT];

typedef struct state_t{
    board board;
    int turn;
} state;

void reset_board(board state){
    for(int i = 0; i < BOARD_WIDTH; i++){
        for(int j = 0; j < BOARD_HEIGHT; j++){
            state[i][j] = 0;
        }
    }
}

void print_board(board state){
    printf("\n");
    for(int j = BOARD_HEIGHT-1; j >= 0; j--){
        for(int i = 0; i < BOARD_WIDTH; i++){
            printf("%i ",state[i][j]);
        }
        printf("\n");
    }
}

int update_board(board state, int playerNumber, int move){
    
    for(int j = 0; j < BOARD_HEIGHT; j++){
        if(state[move][j] == 0){
            state[move][j] = playerNumber;
            return 1;
        }
    }
    return 0;
}

void copy_board(board copy, board copied){
    for(int i = 0; i < BOARD_WIDTH; i++){
        for(int j = 0; j < BOARD_HEIGHT; j++){
            copy[i][j] = copied[i][j];
        }
    }
}

int pattern_row(board state){
    int a,b,c,d;
    for(int i = 0; i < BOARD_WIDTH-3; i++){
        for(int j = 0; j < BOARD_HEIGHT; j++){
            a = state[i][j], b = state[i+1][j], c = state[i+2][j], d = state[i+3][j];
            if(0 != a && a == b && b == c && c == d)
                return 1;
        }
    }
    return 0;
}

int pattern_col(board state){
    int a,b,c,d;
    for(int i = 0; i < BOARD_WIDTH; i++){
        for(int j = 0; j < BOARD_HEIGHT-3; j++){
            a = state[i][j], b = state[i][j+1], c = state[i][j+2], d = state[i][j+3];
            if(0 != a && a == b && b == c && c == d)
                return 1;
        }
    }
    return 0;
}

int pattern_diag(board state){
    int a,b,c,d;
    for(int i = 0; i < BOARD_WIDTH-3; i++){
        for(int j = 0; j < BOARD_HEIGHT-3; j++){
            a = state[i][j], b = state[i+1][j+1], c = state[i+2][j+2], d = state[i+3][j+3];
            if(0 != a && a == b && b == c && c == d)
                return 1;
        }
    }
    return 0;
}

int pattern_rdiag(board state){
    int a,b,c,d;
    for(int i = 0; i < BOARD_WIDTH-3; i++){
        for(int j = 0; j < BOARD_HEIGHT-3; j++){
            a = state[i][j+3], b = state[i+1][j+2], c = state[i+2][j+1], d = state[i+3][j];
            if(0 != a && a == b && b == c && c == d)
                return 1;
        }
    }
    return 0;
}

int game_over(board state){
    if(pattern_row(state) || pattern_col(state) || pattern_diag(state) || pattern_rdiag(state))
        return 1;
    return 0;
}

int random_choice(){
    return rand() % BOARD_WIDTH;
}


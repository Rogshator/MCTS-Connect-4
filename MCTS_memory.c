#pragma once

#include "MCTS_base.c"

typedef struct Node_t
{
    State state;
    float value;
    long exploration;
    int is_leaf; //is_leaf if all moves are NULL
    int allowed_moves[BOARD_WIDTH];
    float next_UCB1[BOARD_WIDTH];
    struct Node_t *next[BOARD_WIDTH];
    struct Node_t *parent;
} Node;

Node *init_node(Node *parent, int move){
    Node *init = malloc(sizeof(Node));
    copy_board(init->state.board, parent->state.board);
    init->state.turn = parent->state.turn + 1;
    init->state.last_player = parent->state.next_player;
    init->state.next_player = parent->state.last_player;
    update_board(init->state.board, init->state.last_player, move); // improve
    init->value = 0;
    init->exploration = 0;
    init->is_leaf = 1;
    for(int i = 0; i < BOARD_WIDTH; i++){
        init->allowed_moves[i] = 0;
        init->next_UCB1[i] = -1;
        init->next[i] = NULL;
    }
    init->parent = parent;
    return init;
}

Node *init_root(){
    Node *init = malloc(sizeof(Node));

    reset_board(init->state.board);
    init->state.turn = 0;
    init->state.last_player = 2;
    init->state.next_player = 1;
    init->value = 0;
    init->exploration = 0;
    init->is_leaf = 1;
    for(int i = 0; i < BOARD_WIDTH; i++){
        init->allowed_moves[i] = 0;
        init->next_UCB1[i] = -1;
        init->next[i] = NULL;
    }
    init->parent = NULL;

    return init;
}

void free_tree(Node *root){

    if(root == NULL) return;
    if(!root->is_leaf){
        for(int i = 0; i < BOARD_WIDTH; i++){
            free_tree(root->next[i]);
        }
    }
    free(root);
    root = NULL;
}
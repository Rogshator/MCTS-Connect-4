#pragma once

#include "MCTS_base.c"
#include "MCTS_memory.c"

#define USB1_max (float)1000000
#define DRAW (float)0
#define WIN (float)1
#define LOSS (float)-1
#define EXPL_WEIGHT 7

int allowed_move(Board board, int move){
    if(!board[move][BOARD_HEIGHT-1]) return 1;
    return 0;
} 

float UCB1(float total_value, int expl_current, int expl_parent, int mean_sign){
    float UCB1;
    if(expl_current == 0) UCB1 = USB1_max;
    else UCB1 = (float)(mean_sign*total_value/expl_current + EXPL_WEIGHT*sqrt(log(expl_parent)/expl_current));
    return UCB1;
}

float max_UCB1(Node *node, int mean_sign){
//return move id with max USB1 value
    float max_UCB1 = -1;
    int max_i = 0;
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(node->allowed_moves[i]){
            node->next_UCB1[i] = UCB1(node->next[i]->value, node->next[i]->exploration, node->exploration, mean_sign);
            if(i == 0 || node->next_UCB1[i] >= max_UCB1){
                max_UCB1 = node->next_UCB1[i];
                max_i = i;
            }
        }
        else{
            node->next_UCB1[i] = -1;
        }
    }
    return max_i;
}

float max_mean_value(Node *root){
    float max_mean = -1;
    int max_i = 0;
    float mean_i;
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(root->allowed_moves[i]){
            mean_i = (float)root->next[i]->value/root->next[i]->exploration;
            if(i == 0 || mean_i >= max_mean){
                max_mean = mean_i;
                max_i = i; 
            }
        }
    }
    return max_i;
}

int simulate_random_game(State state){
    int current_player;
    int choice;
    int updated;
/*     int inverse_player[2] = {2,1};
    int last_player = inverse_player[state.player - 1];
    current_player = inverse_player[last_player - 1];
    faster than
    current_player = i%2+1 ? no */

    for(int i = state.turn; i < MAX_GAME_LENGHT; i++){  
        current_player = i % 2 + 1; //TODO : change % to assignation of players by accessing state 
        do{
        choice = random_choice();
        updated = update_board(state.board, current_player, choice);
        } while(!updated);
        updated = 0;
        if(game_over(state.board)){
            return current_player;
        }
    }
    return 0;
} 


float end_value(int player, int winner){
    if(!winner) return DRAW;
    if(player == winner) return WIN;
    return LOSS;
}

float rollout(Node *leaf, int player){
    int winner = simulate_random_game(leaf->state);
    int value = end_value(player, winner);
    leaf->exploration += 1;
    leaf->value += value;
    return value;
}

void back_propagation(Node *leaf, float value){
    leaf->exploration += 1;
    leaf->value += value;
    while(leaf->parent != NULL){
        leaf = leaf->parent;
        leaf->exploration += 1;
        leaf->value += value;
    }
}

int MCTS(Node *root, int time_limit, int player){
    //return preferred move
    //TODO: Fix not seeing win when ending move


/*     //Check if next move is ending move
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(allowed_move(root->state.board, i)){
            board state;
            copy_board(state, root->state.board);
            update_board(state, player, i);
            if(game_over(state)){
                root->next[i] = init_node(root, i);
                printf("ending move : %i", i);
                return i;
            }
        }
    } */

    //Run MCTS
    Node *branch;
    time_t time_start = time(0);
    int its = 0;
    do{
        branch = root;
        its += 1;
        while(!branch->is_leaf){
                int mean_sign = (branch->state.next_player == player)*2-1;
                int max_i = max_UCB1(branch, mean_sign);
                branch = branch->next[max_i];
        }
        if(!branch->exploration){
            //back propagation
            float value = rollout(branch, player);
            back_propagation(branch, value);
        }
        else{
            if(game_over(branch->state.board)){
                float value = end_value(player, branch->state.last_player);
                back_propagation(branch, value);
                continue;
            }
            //create next nodes + back propagation
            if(branch->state.turn == MAX_GAME_LENGHT){
                back_propagation(branch, DRAW);
                continue;
            }

            int first_allowed_move = 1;
            for(int i = 0; i < BOARD_WIDTH; i++){
                branch->allowed_moves[i] = allowed_move(branch->state.board, i);
                if(branch->allowed_moves[i]){
                    branch->next[i] = init_node(branch, i);
                    if(first_allowed_move){
                        float value = rollout(branch->next[i], player);
                        back_propagation(branch->next[i], value);
                        first_allowed_move = 0;
                    }
                }
            }
            branch->is_leaf = 0;
        }
    } while(time(0) - time_start < time_limit);
    // } while(its < time_limit);
    printf("%i iterations\n", its);
/*    for(int i = 0; i < BOARD_WIDTH; i++){
       if(root->allowed_moves[i]) printf("move : %i, value : %.f, expl_value : %i, mean : %.2f\n", i, root->next[i]->total_value, root->next[i]->expl_value, (float)100*root->next[i]->total_value/root->next[i]->expl_value);
    } */
    int preferred_move = max_mean_value(root);
    printf("player : %i, value : %.2f, move : %i, turn : %i, %.0f over %ld expls\n", player, (float)100*(root->next[preferred_move]->value/root->next[preferred_move]->exploration), preferred_move, root->state.turn, root->next[preferred_move]->value, root->next[preferred_move]->exploration);
    return preferred_move;   
}
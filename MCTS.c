#include "memory.c"
#define USB1_max (float)1000000
#define DRAW 0
#define WIN 1
#define LOSS -1
#define EXPL_WEIGHT 2

int allowed_move(board state, int move){
    for(int j = 0; j < BOARD_HEIGHT; j++){
        if(!state[move][j]) return 1;
    }
    return 0;
} 

float UCB1(float total_value, int expl_current, int expl_parent){
    float UCB1;
    if(expl_current == 0) UCB1 = USB1_max;
    else UCB1 = (float)(total_value/expl_current + EXPL_WEIGHT*sqrt(log(expl_parent)/expl_current));
    return UCB1;
}

float max_UCB1(node *branch){
//return move id with max USB1 value
    float max_UCB1 = -1;
    int max_i = 0;
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(branch->allowed_moves[i]){
            branch->next_UCB1[i] = UCB1(branch->next[i]->total_value, branch->next[i]->expl_value, branch->expl_value);
            if(i == 0 || branch->next_UCB1[i] >= max_UCB1){
                max_UCB1 = branch->next_UCB1[i];
                max_i = i;
            }
        }
        else{
            branch->next_UCB1[i] = -1;
        }
    }
    return max_i;
}

float max_mean_value(node *root){
    float max_mean = -1;
    int max_i = 0;
    float mean_i;
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(root->allowed_moves[i]){
            mean_i = (float)root->next[i]->total_value/root->next[i]->expl_value;
            if(i == 0 || mean_i >= max_mean){
                max_mean = mean_i;
                max_i = i; 
            }
        }
    }
    return max_i;
}

float simulate_random_game(state state){
    int current_player;
    int choice;
    int updated;
    for(int i = state.turn; i < MAX_GAME_LENGHT; i++){
        current_player = i % 2 + 1;
        do{
        choice = random_choice();
        updated = update_board(state.board, current_player, choice);
        } while(!updated);
        updated = 0;
        if(game_over(state.board)){
            state.turn = i;
            return current_player;
        }
    }
    return 0;
} 


int end_value(int player, int winner){
    if(!winner) return DRAW;
    if(player == winner) return WIN;
    return LOSS;
}

float rollout(node *leaf, int player){

    int winner = simulate_random_game(leaf->state);
    int value = end_value(player, winner);
    leaf->expl_value += 1;
    leaf->total_value += value;
    return value;
}

void back_propagation(node *leaf, int value){
    while(leaf->parent != NULL){
        leaf = leaf->parent;
        leaf->expl_value += 1;
        leaf->total_value += value;
    }
}

int MCTS(node *root, int time_limit, int player){
    //return preferred move

    node *branch;

    time_t time_start = time(0);
    int its = 0;
    do{
        its += 1;
        branch = root;
        while(!branch->is_leaf){
            if(branch->state.turn == MAX_GAME_LENGHT){
                branch->is_leaf = 1;
                continue;
            }
            else{
                int max_i = max_UCB1(branch);
                branch = branch->next[max_i];
            }
        }
        if(!branch->expl_value){
            //back propagation
            int value = rollout(branch, player);
            back_propagation(branch, value);
        }
        else{
            //create next nodes + back propagation
            if(branch->state.turn == MAX_GAME_LENGHT){
                branch->expl_value += 1;
                branch->total_value += DRAW;
                back_propagation(branch, DRAW);
                continue;
            }
            int first_allowed_move = 1;
            for(int i = 0; i < BOARD_WIDTH; i++){
                branch->allowed_moves[i] = allowed_move(branch->state.board, i);
                if(branch->allowed_moves[i]){
                    branch->next[i] = init_node(branch, i);
                    if(first_allowed_move){
                        int value = rollout(branch->next[i], player);
                        back_propagation(branch->next[i], value);
                        first_allowed_move = 0;
                    }
                }
            }
            branch->is_leaf = 0;
        }
    } while(time(0) - time_start < time_limit);

    int preferred_move = max_mean_value(root);
    return preferred_move;
    
}
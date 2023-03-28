#include "MCST.c"

void next_move(node *root, int move){
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(move != i && root->allowed_moves[i]){
            free_tree(root->next[i]);
            root->next[i] = NULL;
        }
    }

    root = root->next[move];
    for(int i = 0; i < BOARD_WIDTH; i++){
        if(root->allowed_moves[i]){
            free_tree(root->next[i]);
        }
        root->next[i] = NULL;
        root->allowed_moves[i] = 0;
        root->next_UCB1[i] = -1;
    }

    root->total_value = 0;
    root->expl_value = 0;
    root->is_leaf = 1;

}

int main() {
    //FILE *f = fopen("log.txt", "w");
    int wins[3] = {0, 0, 0};
    srand(time(0));

    printf("----------------------------------------------------------------");

    for (int game = 0; game < 100; game++) {
        node *root = init_root();

        board main_board;
        reset_board(main_board);
        int its = 100000;
        int expl_weight = 2;
        int player;
        int time_limit = 1;

        for (int i = 0; i < MAX_GAME_LENGHT; i++) {
            player = i % 2 + 1;
            int move = MCTS(root, time_limit, expl_weight, player);
            printf("player : %i, value : %.0f, move : %i, turn : %i\n", player, root->total_value, move, root->state.turn);

            next_move(root, move);
            root = root->next[move];
            //printf("turn : %i, move : %i\n", i+1, move);
            //print_board(root->state.board);
            if (game_over(root->state.board) || root->state.turn == MAX_GAME_LENGHT) {
                if (root->state.turn == MAX_GAME_LENGHT) player = 0;
                wins[player] += 1;
                printf("Winner : Player %i, wins: %i/%i\nFinal Board :", player, wins[player], game + 1);
                print_board(root->state.board);

                printf("Total games: %i\nPlayer 1: %i wins\nPlayer 2: %i wins\nDraws: %i\n", game + 1, wins[1], wins[2], wins[0]);
                break;
            }
        }
        //free_tree(root);
    }
    return 0;
}

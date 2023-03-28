#include "MCTS.c"

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

    char game_choice;
    
    do{
        
        scanf("Choose your game mode (S for simulation, P for playing):\n", &game_choice);
    } while(game_choice != "S" && game_choice != "P");

    if(game_choice == "S"){
        //TODO: make things flexible
        
        int wins[3] = {0, 0, 0};
        srand(time(0));

        printf("----------------------------Start of simulation------------------------------------\n");

        for (int game = 0; game < 100; game++)
        {
            node *root = init_root();

            board main_board;
            reset_board(main_board);
            int its = 100000;
            int player;
            int time_limit = 1;

            for (int i = 0; i < MAX_GAME_LENGHT; i++)
            {
                player = i % 2 + 1;
                int move = MCTS(root, time_limit, player);
                printf("player : %i, value : %.0f, move : %i, turn : %i\n", player, root->total_value, move, root->state.turn);

                next_move(root, move);
                root = root->next[move];

                if (game_over(root->state.board) || root->state.turn == MAX_GAME_LENGHT)
                {
                    if (root->state.turn == MAX_GAME_LENGHT)
                        player = 0;
                    wins[player] += 1;
                    printf("Winner : Player %i, wins: %i/%i\nFinal Board :", player, wins[player], game + 1);
                    print_board(root->state.board);

                    printf("Total games: %i\nPlayer 1: %i wins\nPlayer 2: %i wins\nDraws: %i\n", game + 1, wins[1], wins[2], wins[0]);
                    break;
                }
            }
        }
    }
    if(game_choice == "P"){
        //TODO:Make player choose side (random, first player, or 2nd player)
        int real_player;
        int time_limit = 1;

        do{
            scanf("Choose your side (0 for random side):\n", &real_player);
        } while(real_player < 0 || real_player > 2); 
        
        if(!real_player){
            real_player = rand()%2 + 1;
        }

        state board;
        reset_board(board.board);
        board.turn = 0;

        for(board.turn = 0; board.turn < MAX_GAME_LENGHT; board.turn++){
            int current_player = board.turn%2 + 1;
            int move;

            if(current_player == real_player){
                do{
                    scanf("Choose your move (0 to 6):", &move);
                } while(move < 0 || move > BOARD_WIDTH || !allowed_move(board.board, move));
                update_board(board.board, current_player, move);
            }
            else{
                node *root = init_root();
                move = MCTS(root, time_limit, current_player);
                update_board(board.board, current_player, move);
                free_tree(root);
            }

            print_board(board.board);
        }
    }
    return 0;
}

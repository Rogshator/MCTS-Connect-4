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
        printf("Choose your game mode (S for simulation, P for playing) :\n");
        scanf("%c", &game_choice);
    } while(game_choice != 'S' && game_choice != 'P');

    if(game_choice == 'S'){
        //TODO: make things flexible
        
        int wins[3] = {0, 0, 0};
        int max_games;
        int time_limit;

        srand(time(0));
        printf("Set the simulation parameters\n");
        
        do{
            printf("Number of games :\n");
            scanf("%d", &max_games);
        } while(max_games <= 0);
        do{
            printf("Time limit for computation :\n");
            scanf("%d", &time_limit);
        } while(time_limit <= 0);


        printf("----------------------------Start of simulation----------------------------\n");

        for (int game = 0; game < max_games; game++)
        {
            node *root = init_root();
            int current_player;
            int game_path[MAX_GAME_LENGHT];
            for(int i = 0; i < BOARD_WIDTH; i++) game_path[i] = 0;

            for (int i = 0; i < MAX_GAME_LENGHT; i++)
            {
                current_player = i % 2 + 1;
                int move = MCTS(root, time_limit, current_player);
                print_board(root->state.board);
                printf("player : %i, value : %.2f, move : %i, turn : %i, %.0f over %i expls\n", current_player, (float)100*(root->next[move]->total_value/root->next[move]->expl_value), move, root->state.turn, root->next[move]->total_value, root->next[move]->expl_value);
                game_path[i] = move;
                next_move(root, move);
                root = root->next[move];

                if (game_over(root->state.board) || root->state.turn == MAX_GAME_LENGHT)
                {
                    if (root->state.turn == MAX_GAME_LENGHT)
                        current_player = 0;
                    wins[current_player] += 1;
                    printf("Winner : Player %i, wins: %i/%i\nFinal Board :", current_player, wins[current_player], game + 1);
                    print_board(root->state.board);

                    printf("Total games: %i\nPlayer 1: %i wins\nPlayer 2: %i wins\nDraws: %i\n", game + 1, wins[1], wins[2], wins[0]);
                    printf("game path : \n");
                    for(int i = 0; i < MAX_GAME_LENGHT; i++) printf("%i ", game_path[i]);
                    printf("\n");                                        
                    break;
                }
            }
        }
        printf("----------------------------End of simulation----------------------------\n");
    }

    if(game_choice == 'P'){
        //TODO:Make player choose side (random, first player, or 2nd player)
        int real_player;
        int time_limit = 1;

        do{
            printf("Choose your side (0 for random side) :\n");
            scanf("%d", &real_player);
        } while(real_player < 0 || real_player > 2); 
        
        if(!real_player){
            real_player = rand()%2 + 1;
        }

        state game_board;
        reset_board(game_board.board);
        game_board.turn = 0;

        for(game_board.turn = 0; game_board.turn < MAX_GAME_LENGHT; game_board.turn++){
            int current_player = game_board.turn%2 + 1;
            int move;

            if(current_player == real_player){
                do{
                    printf("Choose your move (1 to 7) :\n");
                    scanf("%d", &move);
                } while(move < 1 || move > BOARD_WIDTH+1 || !allowed_move(game_board.board, move-1));
                update_board(game_board.board, current_player, move-1);
            }
            else{
                node *root = init_root();
                root->state.turn = game_board.turn;
                copy_board(root->state.board, game_board.board);

                move = MCTS(root, time_limit, current_player);
                printf("It played %i", move+1);
                update_board(game_board.board, current_player, move);
                free_tree(root);
            }
            print_board(game_board.board);
            if(game_over(game_board.board)){
                if(game_board.turn == MAX_GAME_LENGHT-1){
                    printf("Draw!");
                }
                if(current_player == real_player){
                    printf("You win!");
                }
                else{
                    printf("You lose!");
                }
                break;
            }
        }
    }
    return 0;
}

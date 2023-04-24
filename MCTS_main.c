#include "MCTS_base.c"
#include "MCTS_memory.c"
#include "MCTS_JSONexport.c"
#include "MCTS.c"


void next_move(Node *root, int move){
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

    root->value = 0;
    root->exploration = 0;
    root->is_leaf = 1;

}

int main() {

    char game_choice;
    
    do{
        printf("Choose your game mode (S for simulation, P for playing, F for first move) :\n");
        scanf(" %c", &game_choice);
    } while(game_choice != 'S' && game_choice != 'P' && game_choice != 'F');

    if(game_choice == 'S'){
        //TODO: make things flexible
        
        int wins[3] = {0, 0, 0};

        srand(time(0));
        printf("Set the simulation parameters\n");
        
        int max_games = 0;
        do{
            printf("Number of games :\n");
            scanf(" %d", &max_games);
        } while(max_games <= 0);

        int time_limit = 0;
        do{
            printf("Time limit for computation :\n");
            scanf(" %d", &time_limit);
        } while(time_limit <= 0);
        
        char export_JSON = 'n';
        do{
            printf("Export to JSON? (Y/n) :\n");
            scanf(" %c", &export_JSON);
        } while(export_JSON != 'Y' && export_JSON != 'n');

        printf("----------------------------Start of simulation----------------------------\n");

        for (int game = 0; game < max_games; game++)
        {
            Node *root = init_root();
            Node *start = root;
            int current_player;
            int game_path[MAX_GAME_LENGHT];
            for(int i = 0; i < MAX_GAME_LENGHT; i++) game_path[i] = -1;

            for (int i = 0; i < MAX_GAME_LENGHT; i++)
            {
                current_player = i % 2 + 1;
                int move = MCTS(root, time_limit, current_player);
                // print_board(root->state.board);
                
                if(export_JSON == 'Y') to_JSON(root, game);
                
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
                    for(int i = 0; i < root->state.turn + 1; i++) printf("%i ", game_path[i]);
                    printf("\n");

                    free_tree(start);                                    
                    break;
                }
            }
        }
        printf("----------------------------End of simulation----------------------------\n");
    }

    if(game_choice == 'P'){
        //TODO:Make player choose side (random, first player, or 2nd player)
        int time_limit = 2;

        int user = 0;
        do{
            printf("Choose your side (0 for random side) :\n");
            scanf(" %d", &user);
        } while(user < 0 || user > 2); 
        
        if(!user){
            user = rand()%2 + 1;
            printf("You are player %d\n", user);
        }

        State game_board;
        reset_board(game_board.board);
        game_board.turn = 0;

        for(game_board.turn = 0; game_board.turn < MAX_GAME_LENGHT; game_board.turn++){
            int current_player = game_board.turn%2 + 1;

            if(current_player == user){
                int move;
                do{
                    printf("Choose your move (1 to 7) :\n");
                    scanf(" %d", &move);
                } while(move < 1 || move > BOARD_WIDTH+1 || !allowed_move(game_board.board, move-1));
                update_board(game_board.board, current_player, move-1);
            }
            else{
                Node *root = init_root();
                root->state.turn = game_board.turn;
                copy_board(root->state.board, game_board.board);

                int move = MCTS(root, time_limit, current_player);
                printf("It played %i", move+1);
                update_board(game_board.board, current_player, move);
                free_tree(root);
            }
            print_board(game_board.board);
            
            if(game_over(game_board.board)){
                if(game_board.turn == MAX_GAME_LENGHT-1){
                    printf("Draw!");
                }
                if(current_player == user){
                    printf("You win!");
                }   
                else{
                    printf("You lose!");
                }
                break;
            }
        }
    }

    if(game_choice == 'F'){
        int time_limit = 0;
        do{
            printf("Time limit for computation :\n");
            scanf(" %d", &time_limit);
        } while(time_limit <= 0);

        char export_JSON = 'n';
        do{
            printf("Export to JSON? (Y/n) :\n");
            scanf(" %c", &export_JSON);
        } while(export_JSON != 'Y' && export_JSON != 'n');

        Node *root = init_root();
        MCTS(root, time_limit, 1);

        if(export_JSON == 'Y'){
            printf("Exporting...\n");
            to_JSON(root, 0);        
        }

        free_tree(root);
        printf("Done");
    }
    return 0;
}

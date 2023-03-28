#include "base.c"

typedef struct node_t
{
    state state;
    float total_value;
    int expl_value;
    int is_leaf; //is_leaf if all moves are NULL
    int allowed_moves[BOARD_WIDTH];
    float next_UCB1[BOARD_WIDTH];
    struct node_t *next[BOARD_WIDTH];
    struct node_t *parent;
} node;

node *init_node(node *parent, int move){
    node *init = malloc(sizeof(node));
    copy_board(init->state.board, parent->state.board);
    init->state.turn = parent->state.turn + 1;
    update_board(init->state.board, (init->state.turn + 1) % 2 + 1, move); // improve
    init->total_value = 0;
    init->expl_value = 0;
    init->is_leaf = 1;
    for(int i = 0; i < BOARD_WIDTH; i++){
        init->allowed_moves[i] = 0;
        init->next_UCB1[i] = -1;
        init->next[i] = NULL;
    }
    init->parent = parent;
    return init;
}

node *init_root(){
    node *init = malloc(sizeof(node));

    reset_board(init->state.board);
    init->state.turn = 0;
    init->total_value = 0;
    init->expl_value = 0;
    init->is_leaf = 1;
    for(int i = 0; i < BOARD_WIDTH; i++){
        init->allowed_moves[i] = 0;
        init->next_UCB1[i] = -1;
        init->next[i] = NULL;
    }
    init->parent = NULL;

    return init;
}

void free_tree(node *root){

    if(root == NULL) return;
    if(!root->is_leaf){
        for(int i = 0; i < BOARD_WIDTH; i++){
            free_tree(root->next[i]);
        }
    }
    free(root);
    root = NULL;
}
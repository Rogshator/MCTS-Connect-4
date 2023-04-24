#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON/cJSON.c"

#include "MCTS_memory.c"

cJSON* buildJSON(Node* node, int last_move) {
    if (!node) {
        return NULL;
    }

    cJSON* obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "turn", node->state.turn);
    cJSON_AddNumberToObject(obj, "last_move", last_move);
    cJSON_AddNumberToObject(obj, "exploration", node->exploration);
    cJSON_AddNumberToObject(obj, "value", node->value);
    
    cJSON* children = cJSON_CreateArray();
    cJSON_AddItemToObject(obj, "children", children);
    if(!node->is_leaf){
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            if (node->allowed_moves)
            {
                cJSON *child = buildJSON(node->next[i], i);
                cJSON_AddItemToArray(children, child);
            }
        }
    }
    return obj;
}

void to_JSON(Node *root, int game){
        // build a JSON object for the root node
    cJSON* json = buildJSON(root, -1);

    // convert the JSON object to a JSON string
    char* json_str = cJSON_Print(json);

    
    char path[50] = "out/MCTStree";
    char turn_str[10];
    char game_str[10];
    sprintf(turn_str, "%d", root->state.turn);
    sprintf(game_str, "%d", game);
    strcat(path, game_str);
    strcat(path, "_");
    strcat(path, turn_str);
    strcat(path, ".json");

    // write the JSON string to a file
    FILE* fp = fopen(path, "w");
    fprintf(fp, "%s", json_str);
    fclose(fp);

    cJSON_Delete(json);
    free(json_str);
}
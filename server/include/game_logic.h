#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "round_manager.h"

typedef enum {
    BOTH_CORRECT,
    ONLY_P1,
    ONLY_P2,
    NONE_CORRECT
} GameResult;

typedef struct {
    RoundManager rm;
    char secret_word[6];
    int last_result[5];
    int player_results[2];
    int secret_word_set;
    int game_active;
} GameState;

void init_game(GameState *game);
int set_secret_word(GameState *game, const char *word);
int secret_word_ready(const GameState *game);
int process_guess(GameState *game, const char *guess, int result[5]);
int current_round_won(const int result[5]);
int current_round_over(const GameState *game, const int result[5]);
void register_round_result(GameState *game, int guessing_player, int guessed_correctly);
void clear_secret_word(GameState *game);
void reset_last_result(GameState *game);
void prepare_next_round(GameState *game);
int game_over(const GameState *game);
GameResult final_result(const GameState *game);
void end_game(GameState *game);

#endif

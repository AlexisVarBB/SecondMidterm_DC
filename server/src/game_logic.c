#include "../include/game_logic.h"
#include "../include/word_eval.h"
#include <string.h>

#define WORD_LENGTH 5

void init_game(GameState *game) {
    int i;

    if (game == NULL) {
        return;
    }

    init_round_manager(&game->rm);

    for (i = 0; i < 6; i++) {
        game->secret_word[i] = '\0';
    }

    reset_last_result(game);

    game->player_results[0] = 0;
    game->player_results[1] = 0;
    game->secret_word_set = 0;
    game->game_active = 1;
}

int set_secret_word(GameState *game, const char *word) {
    char temp_word[6];

    if (game == NULL || word == NULL) {
        return 0;
    }

    if (!is_valid_word(word)) {
        return 0;
    }

    strncpy(temp_word, word, WORD_LENGTH);
    temp_word[WORD_LENGTH] = '\0';

    normalize_word(temp_word);

    strcpy(game->secret_word, temp_word);
    game->secret_word_set = 1;

    return 1;
}

int secret_word_ready(const GameState *game) {
    if (game == NULL) {
        return 0;
    }

    return game->secret_word_set;
}

int process_guess(GameState *game, const char *guess, int result[5]) {
    char temp_guess[6];
    int i;

    if (game == NULL || guess == NULL || result == NULL) {
        return 0;
    }

    if (!secret_word_ready(game)) {
        return 0;
    }

    if (!is_valid_word(guess)) {
        return 0;
    }

    strncpy(temp_guess, guess, WORD_LENGTH);
    temp_guess[WORD_LENGTH] = '\0';

    normalize_word(temp_guess);

    evaluate_guess(game->secret_word, temp_guess, result);

    for (i = 0; i < WORD_LENGTH; i++) {
        game->last_result[i] = result[i];
    }

    increment_attempt(&game->rm);

    return 1;
}

int current_round_won(const int result[5]) {
    return is_correct_guess(result);
}

int current_round_over(const GameState *game, const int result[5]) {
    if (game == NULL || result == NULL) {
        return 0;
    }

    if (current_round_won(result)) {
        return 1;
    }

    if (max_attempts_reached(&game->rm)) {
        return 1;
    }

    return 0;
}

void register_round_result(GameState *game, int guessing_player, int guessed_correctly) {
    if (game == NULL) {
        return;
    }

    if (guessing_player < 0 || guessing_player > 1) {
        return;
    }

    game->player_results[guessing_player] = guessed_correctly ? 1 : 0;
}

void clear_secret_word(GameState *game) {
    int i;

    if (game == NULL) {
        return;
    }

    for (i = 0; i < 6; i++) {
        game->secret_word[i] = '\0';
    }

    game->secret_word_set = 0;
}

void reset_last_result(GameState *game) {
    int i;

    if (game == NULL) {
        return;
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        game->last_result[i] = -1;
    }
}

void prepare_next_round(GameState *game) {
    if (game == NULL) {
        return;
    }

    if (all_rounds_finished(&game->rm)) {
        return;
    }

    clear_secret_word(game);
    reset_last_result(game);
    switch_roles(&game->rm);
    advance_round(&game->rm);
    start_round(&game->rm, game->rm.setter_id, game->rm.guesser_id);
}

int game_over(const GameState *game) {
    if (game == NULL) {
        return 0;
    }

    return all_rounds_finished(&game->rm);
}

GameResult final_result(const GameState *game) {
    if (game == NULL) {
        return NONE_CORRECT;
    }

    if (game->player_results[0] == 1 && game->player_results[1] == 1) {
        return BOTH_CORRECT;
    }

    if (game->player_results[0] == 1 && game->player_results[1] == 0) {
        return ONLY_P1;
    }

    if (game->player_results[0] == 0 && game->player_results[1] == 1) {
        return ONLY_P2;
    }

    return NONE_CORRECT;
}

void end_game(GameState *game) {
    if (game == NULL) {
        return;
    }

    game->game_active = 0;
}

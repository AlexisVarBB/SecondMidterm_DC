#include "../include/word_eval.h"
#include <ctype.h>
#include <string.h>

#define WORD_LENGTH 5

int has_repeated_letters(const char *word) {
    int i, j;

    if (word == NULL) {
        return 0;
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        for (j = i + 1; j < WORD_LENGTH; j++) {
            if (toupper((unsigned char)word[i]) == toupper((unsigned char)word[j])) {
                return 1;
            }
        }
    }

    return 0;
}

int is_valid_word(const char *word) {
    int i;

    if (word == NULL) {
        return 0;
    }

    if (strlen(word) != WORD_LENGTH) {
        return 0;
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        if (!isalpha((unsigned char)word[i])) {
            return 0;
        }
    }

    if (has_repeated_letters(word)) {
        return 0;
    }

    return 1;
}

void normalize_word(char *word) {
    int i;

    if (word == NULL) {
        return;
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        word[i] = toupper((unsigned char)word[i]);
    }
}

void evaluate_guess(const char *secret, const char *guess, int result[5]) {
    int i, j;
    int matched_secret[5] = {0, 0, 0, 0, 0};

    for (i = 0; i < WORD_LENGTH; i++) {
        result[i] = 0;
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == secret[i]) {
            result[i] = 2;
            matched_secret[i] = 1;
        }
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        if (result[i] == 2) {
            continue;
        }

        for (j = 0; j < WORD_LENGTH; j++) {
            if (!matched_secret[j] && guess[i] == secret[j]) {
                result[i] = 1;
                matched_secret[j] = 1;
                break;
            }
        }
    }
}

int is_correct_guess(const int result[5]) {
    int i;

    if (result == NULL) {
        return 0;
    }

    for (i = 0; i < WORD_LENGTH; i++) {
        if (result[i] != 2) {
            return 0;
        }
    }

    return 1;
}

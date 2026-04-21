#ifndef WORD_EVAL_H
#define WORD_EVAL_H

int has_repeated_letters(const char *word);
int is_valid_word(const char *word);
void normalize_word(char *word);
void evaluate_guess(const char *secret, const char *guess, int result[5]);
int is_correct_guess(const int result[5]);

#endif

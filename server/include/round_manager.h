#ifndef ROUND_MANAGER_H
#define ROUND_MANAGER_H

typedef struct {
    int current_round;
    int setter_id;
    int guesser_id;
    int attempts;
    int max_attempts;
    int round_active;
    int rounds_finished;
} RoundManager;

void init_round_manager(RoundManager *rm);
void start_round(RoundManager *rm, int setter_id, int guesser_id);
void increment_attempt(RoundManager *rm);
int attempts_left(const RoundManager *rm);
int max_attempts_reached(const RoundManager *rm);
void end_round(RoundManager *rm);
void switch_roles(RoundManager *rm);
void advance_round(RoundManager *rm);
int is_round_active(const RoundManager *rm);
int is_second_round(const RoundManager *rm);
int all_rounds_finished(const RoundManager *rm);

#endif

#include "../include/round_manager.h"
#include <stddef.h>

#define DEFAULT_MAX_ATTEMPTS 6

void init_round_manager(RoundManager *rm) {
    if (rm == NULL) {
        return;
    }

    rm->current_round = 1;
    rm->setter_id = 0;
    rm->guesser_id = 1;
    rm->attempts = 0;
    rm->max_attempts = DEFAULT_MAX_ATTEMPTS;
    rm->round_active = 0;
    rm->rounds_finished = 0;
}

void start_round(RoundManager *rm, int setter_id, int guesser_id) {
    if (rm == NULL) {
        return;
    }

    rm->setter_id = setter_id;
    rm->guesser_id = guesser_id;
    rm->attempts = 0;
    rm->round_active = 1;
}

void increment_attempt(RoundManager *rm) {
    if (rm == NULL) {
        return;
    }

    rm->attempts++;
}

int attempts_left(const RoundManager *rm) {
    if (rm == NULL) {
        return 0;
    }

    return rm->max_attempts - rm->attempts;
}

int max_attempts_reached(const RoundManager *rm) {
    if (rm == NULL) {
        return 0;
    }

    return rm->attempts >= rm->max_attempts;
}

void end_round(RoundManager *rm) {
    if (rm == NULL) {
        return;
    }

    rm->round_active = 0;
    rm->rounds_finished++;
}

void switch_roles(RoundManager *rm) {
    int temp;

    if (rm == NULL) {
        return;
    }

    temp = rm->setter_id;
    rm->setter_id = rm->guesser_id;
    rm->guesser_id = temp;
}

void advance_round(RoundManager *rm) {
    if (rm == NULL) {
        return;
    }

    rm->current_round++;
}

int is_round_active(const RoundManager *rm) {
    if (rm == NULL) {
        return 0;
    }

    return rm->round_active;
}

int is_second_round(const RoundManager *rm) {
    if (rm == NULL) {
        return 0;
    }

    return rm->current_round == 2;
}

int all_rounds_finished(const RoundManager *rm) {
    if (rm == NULL) {
        return 0;
    }

    return rm->rounds_finished >= 2;
}

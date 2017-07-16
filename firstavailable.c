#include "firstavailable.h"

#ifndef CON4
#include CON4
#endif

int first_move(const struct connect4 *game, int secondsleft) {

    // Get our possible moves.
    int* moves = get_possible_moves(game);
    int i;

    // Take the first one.
    for (i=0; i<NUM_COLS; i++) {
        if (moves[i]) {
            free(moves);
            return i;
        }
    }

    // Should never get here.
    return -1;
}

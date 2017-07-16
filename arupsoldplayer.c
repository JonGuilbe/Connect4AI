#include "arupsoldplayer.h"

// team is who the this player is moving for.
int score (const struct connect4* game) {

    int POINTS[] = {0, 1, 3, 7, 1000000};
    int DX[] = {0,1,1,1}; // cols
    int DY[] = {-1,-1,0,1}; // rows

    int i,j,dir;
    int total = 0;
    int sign = game->whoseTurn == PLAYERONE ? -1 : 1;

    // Look in each possible direction.
    for (dir=0; dir<NUM_DIR; dir++) {

        // Try each starting spot.
        for (i=0; i<NUM_COLS; i++) {
            for (j=0; j<NUM_ROWS; j++) {

                // Calculate the corresponding ending point.
                int lastx = i + DX[dir]*(WIN_STREAK-1);
                int lasty = j + DY[dir]*(WIN_STREAK-1);

                // Not a possible streak.
                if (!inbounds(lastx, lasty)) continue;

                int opp = count_char(game, j, i, DY[dir], DX[dir], WIN_STREAK, PLAYERONE);
                int me = count_char(game, j, i, DY[dir], DX[dir], WIN_STREAK, PLAYERTWO);

                if (opp == 0 || me > 0) total += sign*POINTS[me];
                if (me == 0 && opp > 0) total += sign*POINTS[opp];
            }
        }
    }

    return total;
}

int arup_move(const struct connect4 *game, int secondsleft) {

    int i;
    int status;
    int move;
    int brdscore, maxscore;

    // Here's a copy of the board.
    struct connect4* saveboard = copy(game);
    int* move_list = get_possible_moves(game);

    // Look for a win on this particular move.
    for (i=0;i<NUM_COLS;i++) {

        // Only do valid columns.
        if (move_list[i]) {

            int myrow = get_row(game,i);

            // Put the move on our temp board.
            saveboard->board[myrow][i]= game->whoseTurn;

            // See what the status would be with this move.
            status = check_status(saveboard);

            // This gives "me" a win, return it!
            if (game->whoseTurn == PLAYERONE && status == X_WINS) {
                free(move_list);
                free(saveboard);
                return i;
            }
            if (game->whoseTurn == PLAYERTWO && status == O_WINS) {
                free(move_list);
                free(saveboard);
                return i;
            }

            // Undo this move.
            saveboard->board[myrow][i] = EMPTY;
        }
    }

    // Look to block a potential win.
    for (i=0;i<NUM_COLS;i++) {

        // Look at valid moves only.
        if (move_list[i]) {

            // Pretend other teams goes again =)
            int myrow = get_row(game,i);
            saveboard->board[myrow][i]= other(game->whoseTurn);
            status = check_status(saveboard);

            // We prevent their win by taking their "spot".
            if (game->whoseTurn == PLAYERTWO && status == X_WINS) {
                free(move_list);
                free(saveboard);
                return i;
            }
            if (game->whoseTurn == PLAYERONE && status == O_WINS) {
                free(move_list);
                free(saveboard);
                return i;
            }

            // Undo this move.
            saveboard->board[myrow][i] = EMPTY;
        }
    }

    // Look at the bottom row to see if we have two open sides.
    for (i=0;i<=NUM_COLS-WIN_STREAK;i++) {

        // Yeah, I know this is bad...
        if ((saveboard->board[0][i]=='_') && (saveboard->board[0][i+1]== game->whoseTurn) &&
            (saveboard->board[0][i+2]== game->whoseTurn) && (saveboard->board[0][i+3]=='_')) {

            // Not quite sure what's going on here...I leave one empty spot
            // instead of going for three in a row in these first two cases.
	        if (i == 0 || i == 3)
	            return 4;

            // Here I go for three in a row...
	        else
	            return i;
	    }

    } // end for

    maxscore = -1000;

    // Try out each move if there's no way to win or lose in one turn and
    // return the best scored move.
    for (i=0;i<NUM_COLS;i++) {

        // Look at valid moves only.
        if (move_list[i]) {

            // Try out this move.
            int myrow = get_row(game,i);
            saveboard->board[myrow][i] = game->whoseTurn;
            brdscore = score(saveboard);

            // Update only if we get a strictly better score.
            if (brdscore>maxscore) {
                maxscore = brdscore;
                move = i;
            }

            // Undo this move.
            saveboard->board[get_row(game,i)][i]='_';
        }
    }

    // Clean up.
    free(move_list);
    free(saveboard);

    // This is the move we're going to make.
    return move;
}

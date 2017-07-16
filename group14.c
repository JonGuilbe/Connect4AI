//
// Final Project Group 14
// AI went 6-0, so I didn't really want to touch the code. I did add some nice winning quotes though.
// Also made sure that its first move is in the center since I felt like I was going to get jumped on Wednesday for not making it do so .
//
#include <stdio.h>
#include <stdlib.h>
#include "con4lib.h"
#include "group14.h"

int g14_evaluate(struct connect4 *game, char playerPiece);
int g14_value(int row, int column, int deltar, int deltac, struct connect4 *game, char playerPiece);
int g14_minmaxAB(int depth,struct connect4 *game, char playerPiece, int alpha, int beta);
int g14_checkLoss(struct connect4 *game);
int g14_checkWin(struct connect4 *game);
int g14_isFirstMove(struct connect4 *game);

int g14_move(struct connect4 *game, int secondsleft){
    char playerPiece = game->whoseTurn;
    int i;

    //creates a copy of the current gamestate so that it can safely modify it
    struct connect4 *safeCopy = copy(game);
    //checks to see if it needs to prevent a loss or claim a win before wasting time with the recursion.
    int moveIndex = g14_isFirstMove(safeCopy);
    //obligatory trash talking when a win is found
    if(moveIndex == -1){
        moveIndex = g14_checkWin(safeCopy);
    }
    if(moveIndex != -1){
        char winningPhrases[14][100] = {{"EXODIA, OBLITERATE!!!"},{"I preformed great as always. There was no need for my strategy."},{"THERE IS NO SUCH THING AS PERFECT. BUT IF THERE WAS, IT'S ME!"},
                                        {"pend 5 gg"},{"CIPHER STREAM OF ANNIHILATION"},{"CAN ANYONE PROVIDE ME WITH A DECENT CHALLENGE?!"},{"gg no re"},{"you're too slow!"},{"THIS ISN'T EVEN MY FINAL FORM"},{"always bet group 14"},
                                        {"where u at tho"},{"you're out of your vector!"},{"So zetta slow!"},{"you're...pretty good."}};
        srand(time(NULL));
        int random = (rand() % 15);
        printf("%s\n", winningPhrases[random]);
    }
    if(moveIndex == -1){
        moveIndex = g14_checkLoss(safeCopy);
    }
    //if the AI cannot win or lose in this turn, it starts at each possible move and recursively checks boards from there
    if(moveIndex == -1){
    int bestVal = -9999999;
    for (i = 0; i < 7; ++i) {
        if(game->board[NUM_ROWS-1][i] == '_' ){
            //temporarily places a piece on the board
            move(safeCopy,i,playerPiece);
            int val = g14_minmaxAB(8, safeCopy,other(playerPiece),-999999,999999);
            int rowIndex = get_row(safeCopy, i);
            //removes the temp piece from the board
            safeCopy->board[rowIndex-1][i] = '_';
            //If we got a better value, make this our move.
            if(val >= bestVal){
                bestVal = val;
                moveIndex = i;
            }
            }
        }
    }
    return moveIndex;
}

int g14_isFirstMove(struct connect4 *game){
    int i;
    for (i = 0; i < 7; ++i) {
        if(game->board[0][i] != '_'){
            return -1;
        }
    }
    return 3;
}

//checks to see if the AI is 1 move away from losing.
int g14_checkLoss(struct connect4 *game){
    int i;
    for (i = 0; i < NUM_COLS; ++i) {
        if(game->board[NUM_ROWS-1][i] == '_'){
            move(game,i,other(game->whoseTurn));
            if(check_status(game) == X_WINS && game->whoseTurn != PLAYERONE){
                return i;
            }
            else if(check_status(game) == O_WINS && game->whoseTurn != PLAYERTWO){
                return i;
            }
            int rowIndex = get_row(game, i);
            game->board[rowIndex-1][i] = '_';
        }
    }
    return -1;
}

//checks to see if the AI is 1 move away from winning.
int g14_checkWin(struct connect4 *game){
    int i;
    for (i = 0; i < NUM_COLS; ++i) {
        if(game->board[NUM_ROWS-1][i] == '_'){
            move(game,i,(game->whoseTurn));
            if(check_status(game) == X_WINS && game->whoseTurn == PLAYERONE){
                return i;
            }
            else if(check_status(game) == O_WINS && game->whoseTurn == PLAYERTWO){
                return i;
            }
            int rowIndex = get_row(game, i);
            game->board[rowIndex-1][i] = '_';
        }
    }
    return -1;
}

//Check possible 4-in-a-row locations, score these.
int g14_evaluate(struct connect4 *game, char playerPiece)
{
    int row, column;
    int answer = 0;

    for (row = 3; row < NUM_ROWS; ++row){
        for (column = 0; column < NUM_COLS; ++column){
            answer += g14_value(row, column, -1, 0, game,playerPiece);}}

    for (row = 0; row < NUM_ROWS; ++row){
        for (column = 3; column < NUM_COLS; ++column){
            answer += g14_value(row, column, 0, -1, game,playerPiece);}}

    for (row = 3; row < NUM_ROWS; ++row){
        for (column = 3; column < NUM_COLS; ++column){
            answer += g14_value(row, column, -1, -1, game,playerPiece);}}

    for (row = 3; row < NUM_ROWS; ++row){
        for (column = 0; column <= NUM_COLS-4; ++column){
            answer += g14_value(row, column, -1, +1, game,playerPiece);}}

    return answer;
}

//Helper function for evaluate. Checks to see what kind of streaks we can get on the board and calcs values based on it.
int g14_value(int row, int column, int deltar, int deltac, struct connect4 *game, char playerPiece)
{
    int i;
    int endRow = row + 3*deltar;
    int endColumn = column + 3*deltac;
    int playerCount = 0;
    int opponentCount = 0;
    int ROWS = NUM_ROWS;
    int COLUMNS = NUM_COLS;

    if (
            (row < 0) || (column < 0) || (endRow < 0) || (endColumn < 0)
            ||
            (row >= ROWS) || (endRow >= ROWS)
            ||
            (column >= COLUMNS) || (endColumn >= COLUMNS)
            )
        return 0;

    for (i = 1; i <= 4; ++i)
    {
        if (game->board[row][column] == game->whoseTurn)
            ++playerCount;
        else if (game->board[row][column] != 95){
            ++opponentCount;
            }
        row += deltar;
        column += deltac;
    }

    // if someone has a count of 4, they are a winner. As such, this should return the max possible score. Negative if it's for the opponent.
    if ((playerCount > 0) && (opponentCount > 0))
        return 0;
    else if (playerCount == 4)
        return 24*ROWS*COLUMNS;
    else if (opponentCount == 4)
        return -1*24*ROWS*COLUMNS;
    else
        return playerCount - opponentCount;
}

//minmax algorithm with alpha-beta pruning.
int g14_minmaxAB(int depth,struct connect4 *game, char playerPiece, int alpha, int beta){
    if(depth == 0 || check_status(game) != NOT_OVER){
        return g14_evaluate(game, playerPiece);
    }

    if(playerPiece == game->whoseTurn){
        int minScore = -999999999;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece);
                int currentScore = g14_minmaxAB(depth-1, game, other(playerPiece), alpha, beta);
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                if(currentScore > minScore){
                    minScore = currentScore;
                }
                if(alpha < minScore){
                    alpha = minScore;
                }
                if(beta <= alpha){
                    break;
                }
            }
        }
        return minScore;
    }
    else{
        int maxScore = 999999999;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int currentScore = g14_minmaxAB(depth-1, game, other(playerPiece), alpha,beta);
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                if(currentScore < maxScore){
                    maxScore = currentScore;
                }
                if(beta > maxScore){
                    beta = maxScore;
                }
                if(beta <= alpha){
                    break;
                }
            }
        }
        return maxScore;
    }
}
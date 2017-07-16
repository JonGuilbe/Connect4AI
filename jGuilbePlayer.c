//
// Created by reneg on 4/5/2016.
// Gonna need some sorta minmax, alpha-beta pruning, and a way to assign a value to a move. Won't be that bad since we're checking complete boards?
//
#include <stdio.h>
#include <stdlib.h>
#include "con4lib.h"
#include "jGuilbePlayer.h"
#include "arupsoldplayer.h"

int evaluate(struct connect4 *game, char playerPiece);
int value(int row, int column, int deltar, int deltac, struct connect4 *game, char playerPiece);
int minmaxAB(int depth,struct connect4 *game, char playerPiece, int alpha, int beta);
int amIAboutToLose(struct connect4 *game);
int amIAboutToWin(struct connect4 *game);

int jon_move(struct connect4 *game, int secondsleft){
    char playerPiece = game->whoseTurn;
    int i;

    //creates a copy of the current gamestate so that it can safely modify it
    struct connect4 *safeCopy = copy(game);
    //checks to see if it needs to prevent a loss or claim a win before wasting time with the deep recursion
    int moveIndex = amIAboutToLose(safeCopy);
    if(moveIndex == -1){
        moveIndex = amIAboutToWin(safeCopy);
    }
    if(moveIndex == -1)
        minmaxAB(6,safeCopy,playerPiece,-999999,999999);
    //if the AI cannot win or lose in this turn, it starts at each possible move and recursively checks boards from there
    if(moveIndex == -1){
    int bestVal = -9999999;
    for (i = 0; i < 7; ++i) {
        if(game->board[NUM_ROWS-1][i] == '_' ){
            //temporarily places a piece on the board
            move(safeCopy,i,playerPiece);
            int val = minmaxAB(8, safeCopy,other(playerPiece),-999999,999999);
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

//checks to see if the AI is 1 move away from losing.
int amIAboutToLose(struct connect4 *game){
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
int amIAboutToWin(struct connect4 *game){
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

//I had absolutely no idea how to code a scoring algorithm, so unfortunately I had to look this up. After looking through it, I understood it enough to use for my AI. Modified to take in the connect4 struct and the playerPiece.
int evaluate(struct connect4 *game, char playerPiece)
{
    int row, column;
    int answer = 0;

    // Value moving down from each spot:
    for (row = 3; row < NUM_ROWS; ++row){
        for (column = 0; column < NUM_COLS; ++column){
            answer += value(row, column, -1, 0, game,playerPiece);}}

    // Value moving left from each spot:
    for (row = 0; row < NUM_ROWS; ++row){
        for (column = 3; column < NUM_COLS; ++column){
            answer += value(row, column, 0, -1, game,playerPiece);}}

    // Value heading diagonal (lower-left) from each spot:
    for (row = 3; row < NUM_ROWS; ++row){
        for (column = 3; column < NUM_COLS; ++column){
            answer += value(row, column, -1, -1, game,playerPiece);}}

    // Value heading diagonal (lower-right) from each spot:
    for (row = 3; row < NUM_ROWS; ++row){
        for (column = 0; column <= NUM_COLS-4; ++column){
            answer += value(row, column, -1, +1, game,playerPiece);}}

    return answer;
}

//Helper function for evaluate. Checks to see what kind of streaks we can get on the board and calcs values based on it.
int value(int row, int column, int deltar, int deltac, struct connect4 *game, char playerPiece)
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

//minmax algorithm with alpha-beta pruning. I understand how to code the minmax part but needed some help with the pruning.
int minmaxAB(int depth,struct connect4 *game, char playerPiece, int alpha, int beta){
    if(depth == 0 || check_status(game) != NOT_OVER){
        return evaluate(game, playerPiece);
    }

    if(playerPiece == game->whoseTurn){
        int minScore = -999999999;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int currentScore = minmaxAB(depth-1, game, other(playerPiece), alpha, beta);
                //printf("score set to %d\n", currentScore);
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
                int currentScore = minmaxAB(depth-1, game, other(playerPiece), alpha,beta);
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
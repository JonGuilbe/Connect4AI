//
// Created by reneg on 4/5/2016.
// Gonna need some sorta minmax, alpha-beta pruning, and a way to assign a value to a move. Won't be that bad since we're checking complete boards?
//
#include <stdio.h>
#include <stdlib.h>
#include "con4lib.h"
#include "jGuilbePlayer.h"
#include "arupsoldplayer.h"

int alphabeta(int depth, int alpha, int beta, int maxPlayer, struct connect4 *game, char playerPiece);
int negascout(int depth, int alpha, int beta,struct connect4 *game, char playerPiece);
int evaluate(struct connect4 *game, char playerPiece);
int value(int row, int column, int deltar, int deltac, struct connect4 *game, char playerPiece);
int minmaxPls(int depth,struct connect4 *game, char playerPiece);
int minmaxAB(int depth,struct connect4 *game, char playerPiece, int alpha, int beta);
int copiedscore (const struct connect4* game);
int amIAboutToLose(struct connect4 *game);
int amIAboutToWin(struct connect4 *game);

int moveIndex;

int evaluationTable[6][7] = {{3, 4, 5, 7, 5, 4, 3},
                       {4, 6, 8, 10, 8, 6, 4},
                       {5, 8, 11, 13, 11, 8, 5},
                       {5, 8, 11, 13, 11, 8, 5},
                       {4, 6, 8, 10, 8, 6, 4},
                       {3, 4, 5, 7, 5, 4, 3}};
int max = 9999;

int jon_move(struct connect4 *game, int secondsleft){
    char playerPiece = game->whoseTurn;
    //int bestIndex = 0, bestScore = -9999999;
    //int* moves = get_possible_moves(game);
    int i;
    struct connect4 *safeCopy = copy(game);
    //int bestScore = -999999999;
    //minmaxPls(7,game,playerPiece);
    moveIndex = amIAboutToLose(safeCopy);
    if(moveIndex == -1){
        moveIndex = amIAboutToWin(safeCopy);
    }
    if(moveIndex == -1)
        minmaxAB(6,safeCopy,playerPiece,-999999,999999);

    if(moveIndex == -1){
    int bestVal = -9999999;
    for (i = 0; i < 7; ++i) {
        if(game->board[NUM_ROWS-1][i] == '_' ){
            //firstRun = 1;
            //alphabeta(8,-999999,999999,1,&safeCopy, playerPiece);
    //int tempVal = negascout(12,-max,max,&safeCopy, playerPiece);
            //printf("Ran alphabeta! Tempval is %d, stepCount is no\n", tempVal);
            //if(tempVal > bestScore){
                //printf("temp val was updated, apparently this isn't happening?");
                //bestScore = tempVal;
                //moveIndex = i;
            move(safeCopy,i,playerPiece);
            int val = minmaxAB(8, safeCopy,other(playerPiece),-999999,999999);
            int rowIndex = get_row(safeCopy, i);
            safeCopy->board[rowIndex-1][i] = '_';
            if(val >= bestVal){
                bestVal = val;
                moveIndex = i;
            }
            }
        }
    }
    //printf("Best score is %d and bestIndex is %d\n", bestScore, bestIndex);
    //return bestIndex;
    printf("Move index is: %d\n", moveIndex);
    return moveIndex;
}

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

int calcValue(struct connect4 *game, char playerPiece){
    int gameState = check_status(game);

    if(gameState != NOT_OVER){
        if(gameState == CATS){
            printf("TIE BOARD\n");
            return 1000;}

        //printf("WINNING BOARD\n");
        //print_board(game);
        return max;
    }

    int sum = 0;
    int dir,i,j, k;

    int DX[] = {0,1,1,1}; // cols
    int DY[] = {-1,-1,0,1}; // rows

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

                for(k = 2; k < 4; k++){
                        sum += k * (count_char(game, j, i, DY[dir], DX[dir], k, playerPiece));
                }
            }
        }
    }
    //if(sum == 0){
    //printf("SCORE OF %d BOARD\n", sum);
    //print_board(game);//}


    return sum; //work in progress.
}

/*int alphabeta(int depth, int alpha, int beta, int maxPlayer, struct connect4 *game, char playerPiece){ //is move needed here?
    //stepCount++;
    if(depth == 0 || check_status(game) != NOT_OVER){
        //printf("Found a calculate condition! Calculating value!\n");
        return evaluate(game, playerPiece);

    }
    if(maxPlayer){
        int value = -9999999;

        //for every valid move from the current move, use a dy/dx array?
        int i;
        for(i = 0; i < 7; i++){ //replace with getPossibleMoves?
            if(game->board[NUM_ROWS-1][i] == '_'){//change
                move(game,i,playerPiece); //will probably need to be fixed?
                int alphabetaresult = alphabeta(depth - 1, alpha, beta, 0, game, other(playerPiece));
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_'; //undo move
                if(value < alphabetaresult){
                    value = alphabetaresult;
                }
                if(alpha < value){
                    alpha = value;
                }
                if(beta <= alpha){
                    firstRun = 0;
                    break;
                }
                }

        }
        firstRun = 0;
        return value;
    }
    else{
        int value = 9999999;
        //same as above
        int i;
        for(i = 0; i < 7; i++){ //change
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int alphabetaresult = alphabeta(depth - 1, alpha, beta, 1, game, other(playerPiece));
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                if(value > alphabetaresult){
                    value = alphabetaresult;
                }
                if(beta > value){
                    beta = value;
                }
                if(beta <= alpha){
                    break;
                }

            }
        }
        return value;
    }
} */

int negascout(int depth, int alpha, int beta,struct connect4 *game, char playerPiece){ //issue is with negascout?
    if(depth == 0 || check_status(game) != NOT_OVER){
        //printf("Found a calculate condition! Calculating value!\n");
        //return calcValue(game, playerPiece);
        //return evaluate(game, playerPiece);
        return score(game);
    }
    int i, a = alpha, b = beta, childCount = 0, score, bestScore = -max, bestMove = -1;
    for (i = 0; i < NUM_COLS; ++i) {
        if(game->board[NUM_ROWS-1][i] == '_' ){
            childCount++;
            move(game,i,playerPiece); //will probably need to be fixed?
            //score = -negascout(depth-1,(-a)-1,-a,game,other(playerPiece));
            if((score > a) && (score < b) && ( childCount > 1)){
                score = -negascout(depth-1, (-a)-1, -score,game,other(playerPiece));
            }
            else{
                score = -negascout(depth-1, b, -score,game,other(playerPiece));
            }
            //if(score > bestScore){
              //  bestScore = score;
                //bestMove = i;
            //}
            int rowIndex = get_row(game, i);
            game->board[rowIndex-1][i] = '_';
            if(a < score){
                a = score;
            }
            if(a >= beta)
                break;

        }

    }
    //printf("returning %d\n", a);
    //moveIndex = bestMove;
    return a;
}

int calcValCopied(struct connect4 *game){
    int *p1Scores = calloc(4,sizeof(int));
    int *p2Scores = calloc(4,sizeof(int));

    if(check_status(game) != NOT_OVER){
        if(check_status(game) == CATS){
            return 10000;
        }
        else
            return max;
    }

    return 0;
}

int evaluate(struct connect4 *game, char playerPiece)
{
    // NOTE: Positive answer is good for the computer.
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

    //printf("answer is %d for playerPiece %c\n", answer, playerPiece);
    return answer;
}

int value(int row, int column, int deltar, int deltac, struct connect4 *game, char playerPiece)
{
    // NOTE: Positive return value is good for the computer.
    int i;
    int endRow = row + 3*deltar;
    int endColumn = column + 3*deltac;
    int playerCount= 0;
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
            //printf("Adding to opponent count! it is now %d player piece is %d compared to %d row is %d and column is %d\n", opponentCount, other(playerPiece), game->board[row][column], row, column);
            //print_board(game);
            }
        row += deltar;
        column += deltac;
    }

    //printf("playercount is %d\n", playerCount);
    //printf("opponentcount is %d\n", opponentCount);
    if ((playerCount > 0) && (opponentCount > 0))
        return 0; // Neither player can get four-in-a-row here.
    else if (playerCount == 4)
        return 24*ROWS*COLUMNS;
    else if (opponentCount == 4)
        return -1*24*ROWS*COLUMNS;
    else
        return playerCount - opponentCount;
}

int minmaxPls(int depth,struct connect4 *game, char playerPiece){ //this is working maybe? at 7 steps or more...now time to try minmax with alphabeta pruning so I can try like 10 steps ahead?
    if(depth == 0 || check_status(game) != NOT_OVER){
        return score(game);
        //return evaluate(game, playerPiece);
    }
    int index;
    if(playerPiece == game->whoseTurn){
        int minScore = -999999;
         index = -1;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int currentScore = minmaxPls(depth-1, game, other(playerPiece));
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                if(currentScore < minScore){
                    moveIndex = i;
                    minScore = currentScore;
                }
            }
        }
        return minScore;
    }
    else{
        int maxScore = 999999;
        index = -1;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int currentScore = minmaxPls(depth-1, game, other(playerPiece));
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                if(currentScore > maxScore){
                    moveIndex = i;
                    maxScore = currentScore;
                }
            }
        }
        return maxScore;
    }
    //return index;
}

int minmaxAB(int depth,struct connect4 *game, char playerPiece, int alpha, int beta){
    if(depth == 0 || check_status(game) != NOT_OVER){
        //return copiedscore(game);
        return evaluate(game, playerPiece);
    }

    int index = -1;
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
                    index = i;
                    //printf("index set to %d\n", i);
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
        /*if(index != -1){
            moveIndex = index;
        }*/
        //printf("moveIndex is %d\n", moveIndex);
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
                //if(currentScore > maxScore){
                  //  moveIndex = i;
                    //maxScore = currentScore;
                //}
                if(currentScore < maxScore){
                    //index = i;
                    //printf("max set to %d", currentScore);
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
        //moveIndex = index;
        return maxScore;
    }
    //return index;
}


int minmaxABCopy(int depth,struct connect4 *game, char playerPiece, int alpha, int beta){
    if(depth == 0 || check_status(game) != NOT_OVER){
        //return copiedscore(game);
        return evaluate(game, playerPiece);
    }
    int index = -1;
    if(playerPiece == game->whoseTurn){
        int minScore = -999999;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int currentScore = minmaxAB(depth-1, game, other(playerPiece), alpha, beta);
                //printf("score set to %d\n", currentScore);
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                if(currentScore > minScore){
                    index = i;
                    //printf("index set to %d\n", i);
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
        if(index != -1){
            moveIndex = index;
        }
        //printf("moveIndex is %d\n", moveIndex);
        return minScore;
    }
    else{
        int maxScore = 999999;
        int i;
        for (i = 0; i < 7; ++i) {
            if(game->board[NUM_ROWS-1][i] == '_' ){
                move(game,i,playerPiece); //will probably need to be fixed?
                int currentScore = minmaxAB(depth-1, game, other(playerPiece), alpha,beta);
                int rowIndex = get_row(game, i);
                game->board[rowIndex-1][i] = '_';
                //if(currentScore > maxScore){
                //  moveIndex = i;
                //maxScore = currentScore;
                //}
                if(currentScore < maxScore){
                    //index = i;
                    //printf("max set to %d", currentScore);
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
        //moveIndex = index;
        return maxScore;
    }
    //return index;
}

int copiedscore (const struct connect4* game) {

    if(check_status(game) != NOT_OVER){
        return 99999999;
    }
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
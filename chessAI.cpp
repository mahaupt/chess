//
//  chessAI.cpp
//  chess
//
//  Created by Marcel Haupt on 12.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#include "chessAI.hpp"
#include "board.hpp"
#include "move.hpp"
#include "point.hpp"
#include <algorithm>
#include <cstdlib>


namespace {
int getCenterBonus(const Point &point) {
    int fileDistance = std::abs(point.getX() * 2 - 7);
    int rankDistance = std::abs(point.getY() * 2 - 7);
    return 14 - fileDistance - rankDistance;
}
}

ChessAI::ChessAI():startEbene(2), nodesEvaluated(0)  {};

Move ChessAI::getNextMove(CBoard & board, int color) {
    startEbene = 5;
    nodesEvaluated = 0;
    Move move = Move();
    doAllMoves(board, color, startEbene, move);
    return move;
}


int ChessAI::getSearchDepth() const {
    return startEbene;
}


unsigned long long ChessAI::getNodesEvaluated() const {
    return nodesEvaluated;
}


bool ChessAI::playerIsCheckmateOrRemis(CBoard & board, int player) {
    return !board.hasLegalMove(player);
}


bool ChessAI::playerIsCheck(CBoard & board, int player) {
    return board.isInCheck(player);
}



void ChessAI::sortMoves(CBoard & board, std::vector< Move > & moves) {
    for (int i=0; i < moves.size(); i++) {
        CFigure * tofig = board.getFigure(moves[i].getTo());
        CFigure * fromfig = board.getFigure(moves[i].getFrom());
        
        int score = getCenterBonus(moves[i].getTo());
        int tieBreaker = 0;
        if (tofig != 0) {
            score += 100000 + tofig->getValue() * 100;
        }
        if (fromfig != 0) {
            tieBreaker = -fromfig->getValue();
        }
        
        moves[i].setMoveValues(score, tieBreaker);
    }

    std::sort(moves.begin(), moves.end(), [](const Move &lhs, const Move &rhs) {
        if (lhs.getMoveValue1() != rhs.getMoveValue1()) {
            return lhs.getMoveValue1() > rhs.getMoveValue1();
        }

        return lhs.getMoveValue2() > rhs.getMoveValue2();
    });
}



int ChessAI::doAllMoves(CBoard & board, int color, int ebenen, Move & savemove, int alpha, int beta) {
    nodesEvaluated++;
    
    if (ebenen == 0) {
        return board.evaluateBoard(color);
    }
    
    //get all moves
    std::vector< Move > moves = std::vector< Move >();
    board.getLegalMoves(color, moves);
    
    //abort if no moves
    if (moves.size() <= 0) {
        if (board.isInCheck(color)) {
            return -700000 - ebenen;
        }
        return 0;
    }
    
    //Vorsortierung
    sortMoves(board, moves);
    
    //do every move
    int bestValue = alpha;
    
    for (int i=0; i < moves.size(); i++) {
        Move & nextmove = moves[i];
        nextmove.doMove(board);
        Move move = Move();
        int value = -doAllMoves(board, (color==0)?1:0 , ebenen -1, move, -beta, -bestValue);
        nextmove.reverseMove(board);
        
        if (value > bestValue) {
            bestValue = value;
            
            if (value >= beta)
                break;
            
            //save move
            if (ebenen == startEbene) {
                savemove = nextmove;
            }
        }
    }
    
    return bestValue;
}

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
    Move move = Move();
    nodesEvaluated = 0;
    int value = doAllMoves(board, player, 2, move);
    
    if (value <= -700000) {
        return true;
    }
    return false;
}


bool ChessAI::playerIsCheck(CBoard & board, int player) {
    int color = (player == 0)?1:0;
    Move move = Move();
    nodesEvaluated = 0;
    int value = doAllMoves(board, color, 1, move);
    
    if (value >= 700000) {
        return true;
    }
    return false;
}



std::vector<Move*> ChessAI::sortMoves(CBoard & board, std::vector< Move > & moves) {
    std::vector<Move*> sortedMoves = std::vector<Move*>();
    sortedMoves.reserve(moves.size());
    
    for (int i=0; i < moves.size(); i++) {
        CFigure * tofig = board.getFigure(moves[i].getTo());
        CFigure * fromfig = board.getFigure(moves[i].getFrom());
        
        int tovalue = 0;
        int fromvalue = 0;
        if (tofig != 0) {
            tovalue = tofig->getValue();
        }
        if (fromfig != 0) {
            fromvalue = fromfig->getValue();
        }
        
        moves[i].setMoveValues(tovalue, tovalue-fromvalue);
        sortedMoves.push_back(&moves[i]);
    }

    std::sort(sortedMoves.begin(), sortedMoves.end(), [](Move* lhs, Move* rhs) {
        if (lhs->getMoveValue1() != rhs->getMoveValue1()) {
            return lhs->getMoveValue1() > rhs->getMoveValue1();
        }

        return lhs->getMoveValue2() > rhs->getMoveValue2();
    });

    return sortedMoves;
}



int ChessAI::doAllMoves(CBoard & board, int color, int ebenen, Move & savemove, int alpha, int beta) {
    nodesEvaluated++;
    
    //get all moves
    std::vector< Move > moves = std::vector< Move >();
    bool blackking = false;
    bool whiteking = false;
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            CFigure * figure = board.getFigure(i, j);
            if (figure != 0) {
                if (figure->getColor() == color) {
                    Point pt = Point(i, j);
                    figure->getMoves(pt, board, moves);
                }
            }
            
            //check if figure is king
            if (figure != 0 && (!blackking || !whiteking)) {
                if (figure->getType() == FigureType::King) {
                    if (figure->getColor() == 0) blackking = true;
                    if (figure->getColor() == 1) whiteking = true;
                }
            }

        }
    }
    
    //abort if ebenen is zero, no moves, or kings are away
    if (ebenen == 0 || moves.size() <= 0 || !(blackking && whiteking)) {
        return board.evaluateBoard(color) + (int)moves.size();
    }
    
    //Vorsortierung
    std::vector<Move*> sortedMoves = sortMoves(board, moves);
    
    //do every move
    int bestValue = alpha;
    
    for (int i=0; i < sortedMoves.size(); i++) {
        Move & nextmove = *sortedMoves[i];
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

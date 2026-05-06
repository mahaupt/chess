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
#include <chrono>
#include <cstdlib>
#include <future>
#include <stdexcept>


namespace {
int getCenterBonus(const Point &point) {
    int fileDistance = std::abs(point.getX() * 2 - 7);
    int rankDistance = std::abs(point.getY() * 2 - 7);
    return 14 - fileDistance - rankDistance;
}

class SearchTimeout : public std::runtime_error {
public:
    SearchTimeout(): std::runtime_error("search timeout") {}
};

struct RootSearchResult {
    Move move;
    int value = -99999999;
    unsigned long long nodesEvaluated = 0;
    bool completed = false;
    int moveIndex = 0;
};
}

ChessAI::ChessAI():startEbene(1), completedDepth(0), nodesEvaluated(0), timeBudgetSeconds(3.0)  {};

Move ChessAI::getNextMove(CBoard & board, int color) {
    completedDepth = 0;
    nodesEvaluated = 0;
    searchDeadline = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(timeBudgetSeconds));
    
    Move bestMove = Move();
    std::vector<Move> rootMoves = std::vector<Move>();
    board.getLegalMoves(color, rootMoves);
    if (rootMoves.size() <= 0) {
        return bestMove;
    }
    sortMoves(board, rootMoves);
    
    for (int depth = 1; ; depth++) {
        if (std::chrono::steady_clock::now() >= searchDeadline) {
            break;
        }
        startEbene = depth;
        nodesEvaluated++;
        
        Move depthBestMove = rootMoves[0];
        int depthBestValue = -99999999;
        int depthBestMoveIndex = 0;
        
        Move firstMove = rootMoves[0];
        try {
            firstMove.doMove(board);
            Move ignoredMove = Move();
            depthBestValue = -doAllMoves(board, color == 0 ? 1 : 0, depth - 1, ignoredMove);
            firstMove.reverseMove(board);
        } catch (const SearchTimeout&) {
            firstMove.reverseMove(board);
            break;
        }
        
        std::vector<std::future<RootSearchResult>> futures = std::vector<std::future<RootSearchResult>>();
        futures.reserve(rootMoves.size() - 1);
        
        for (int i = 1; i < rootMoves.size(); i++) {
            Move rootMove = rootMoves[i];
            auto deadline = searchDeadline;
            int rootAlpha = depthBestValue;
            
            futures.push_back(std::async(std::launch::async, [this, &board, color, depth, rootMove, deadline, rootAlpha, i]() mutable {
                RootSearchResult result;
                result.move = rootMove;
                result.moveIndex = i;
                
                ChessAI worker;
                worker.searchDeadline = deadline;
                worker.startEbene = depth - 1;
                worker.nodesEvaluated = 0;
                
                CBoard boardCopy = board;
                Move moveOnCopy = rootMove;
                moveOnCopy.doMove(boardCopy);
                
                try {
                    Move ignoredMove = Move();
                    result.value = -worker.doAllMoves(boardCopy, color == 0 ? 1 : 0, depth - 1, ignoredMove, -99999999, -rootAlpha);
                    result.completed = true;
                } catch (const SearchTimeout&) {
                    result.completed = false;
                }
                
                result.nodesEvaluated = worker.nodesEvaluated;
                return result;
            }));
        }
        
        bool depthCompleted = true;
        
        for (int i = 0; i < futures.size(); i++) {
            RootSearchResult result = futures[i].get();
            nodesEvaluated += result.nodesEvaluated;
            
            if (!result.completed) {
                depthCompleted = false;
                continue;
            }
            
            if (result.value > depthBestValue
                || (result.value == depthBestValue && result.moveIndex < depthBestMoveIndex)) {
                depthBestValue = result.value;
                depthBestMove = result.move;
                depthBestMoveIndex = result.moveIndex;
            }
        }
        
        if (!depthCompleted) {
            break;
        }
        
        bestMove = depthBestMove;
        completedDepth = depth;
        
        if (std::chrono::steady_clock::now() >= searchDeadline) {
            break;
        }
    }
    
    return bestMove;
}


int ChessAI::getSearchDepth() const {
    return completedDepth;
}


unsigned long long ChessAI::getNodesEvaluated() const {
    return nodesEvaluated;
}


double ChessAI::getTimeBudgetSeconds() const {
    return timeBudgetSeconds;
}


void ChessAI::setTimeBudgetSeconds(double seconds) {
    if (seconds > 0) {
        timeBudgetSeconds = seconds;
    }
}


void ChessAI::checkSearchTime() {
    if (std::chrono::steady_clock::now() >= searchDeadline) {
        throw SearchTimeout();
    }
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
    checkSearchTime();
    
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
        int value = 0;
        try {
            value = -doAllMoves(board, (color==0)?1:0 , ebenen -1, move, -beta, -bestValue);
        } catch (const SearchTimeout&) {
            nextmove.reverseMove(board);
            throw;
        }
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

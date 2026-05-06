//
//  chessAI.hpp
//  chess
//
//  Created by Marcel Haupt on 12.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once
#include <chrono>
#include <vector>

class Move;
class CBoard;

class ChessAI {
private:
    int startEbene;
    int completedDepth;
    unsigned long long nodesEvaluated;
    double timeBudgetSeconds;
    std::chrono::steady_clock::time_point searchDeadline;
    int doAllMoves(CBoard & board, int color, int ebenen, Move & savemove, int alpha=-99999999, int beta=99999999);
    void sortMoves(CBoard & board, std::vector< Move > & moves);
    void checkSearchTime();
public:
    ChessAI();
    Move getNextMove(CBoard & board, int color);
    int getSearchDepth() const;
    unsigned long long getNodesEvaluated() const;
    double getTimeBudgetSeconds() const;
    void setTimeBudgetSeconds(double seconds);
};

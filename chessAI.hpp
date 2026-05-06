//
//  chessAI.hpp
//  chess
//
//  Created by Marcel Haupt on 12.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

class Move;
class CBoard;

class ChessAI {
private:
    enum class TranspositionBound {
        Exact,
        Lower,
        Upper
    };
    
    struct TranspositionEntry {
        std::uint64_t key = 0;
        int depth = 0;
        int value = 0;
        TranspositionBound bound = TranspositionBound::Exact;
        bool occupied = false;
    };
    
public:
    struct TranspositionStats {
        unsigned long long probes = 0;
        unsigned long long hits = 0;
        unsigned long long exactHits = 0;
        unsigned long long boundCutoffs = 0;
        unsigned long long stores = 0;
    };
    
private:
    int startEbene;
    int completedDepth;
    unsigned long long nodesEvaluated;
    double timeBudgetSeconds;
    std::chrono::steady_clock::time_point searchDeadline;
    std::vector<TranspositionEntry> transpositionTable;
    TranspositionStats transpositionStats;
    std::size_t getTranspositionIndex(std::uint64_t positionHash) const;
    void storeTransposition(std::uint64_t positionHash, int depth, int value, TranspositionBound bound);
    int doAllMoves(CBoard & board, int color, int ebenen, Move & savemove, int alpha=-99999999, int beta=99999999);
    void sortMoves(CBoard & board, std::vector< Move > & moves);
    void checkSearchTime();
public:
    ChessAI();
    Move getNextMove(CBoard & board, int color);
    int getSearchDepth() const;
    unsigned long long getNodesEvaluated() const;
    const TranspositionStats & getTranspositionStats() const;
    double getTimeBudgetSeconds() const;
    void setTimeBudgetSeconds(double seconds);
};

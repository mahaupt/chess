//
//  chessAI.hpp
//  chess
//
//  Created by Marcel Haupt on 12.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once
#include <vector>
#include "heap.hpp"

class Move;
class CBoard;

class ChessAI {
private:
    int startEbene;
    int doAllMoves(CBoard & board, int color, int ebenen, Move & savemove, int alpha=-99999999, int beta=99999999);
    void sortMoves(CBoard & board, std::vector< Move > & input, Heap < Move > & output);
public:
    ChessAI();
    Move getNextMove(CBoard & board, int color);
    int getSearchDepth() const;
    bool playerIsCheckmateOrRemis(CBoard & board, int player);
    bool playerIsCheck(CBoard & board, int player);
    
    struct sort_values {
        int a = 0;
        int b = 0;
    };
};

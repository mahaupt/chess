#include "benchmark.hpp"

#include "board.hpp"
#include "chessAI.hpp"
#include "chessConstants.hpp"
#include "move.hpp"
#include "moveFormat.hpp"
#include "searchStats.hpp"
#include <chrono>
#include <iostream>
#include <vector>


int runBestMove(const std::string &fen, double timeBudgetSeconds)
{
    CBoard board;
    if (!board.loadFEN(fen == "startpos" ? startingFEN : fen)) {
        std::cerr << "Invalid FEN" << std::endl;
        return 1;
    }
    
    int color = board.getSideToMove();
    std::vector<Move> legalMoves;
    board.getLegalMoves(color, legalMoves);
    if (legalMoves.empty()) {
        std::cout << "bestmove none" << std::endl;
        return 0;
    }
    
    ChessAI ai;
    ai.setTimeBudgetSeconds(timeBudgetSeconds);
    
    auto start = std::chrono::steady_clock::now();
    Move bestMove = ai.getNextMove(board, color);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "bestmove " << formatMove(bestMove) << std::endl;
    printSearchStats(ai, elapsed.count(), std::cout);
    
    return 0;
}

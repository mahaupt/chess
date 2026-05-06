#include "perft.hpp"

#include "board.hpp"
#include "chessConstants.hpp"
#include "move.hpp"
#include "moveFormat.hpp"
#include <chrono>
#include <iostream>
#include <vector>


unsigned long long perft(CBoard &board, int color, int depth)
{
    if (depth == 0) {
        return 1;
    }
    
    std::vector<Move> moves;
    board.getLegalMoves(color, moves);
    if (depth == 1) {
        return moves.size();
    }
    
    unsigned long long nodes = 0;
    for (Move &move : moves) {
        move.doMove(board);
        board.switchSideToMove();
        nodes += perft(board, color == 0 ? 1 : 0, depth - 1);
        board.switchSideToMove();
        move.reverseMove(board);
    }
    
    return nodes;
}


int runPerft(const std::string &fen, int depth, bool divide)
{
    CBoard board;
    if (!board.loadFEN(fen == "startpos" ? startingFEN : fen)) {
        std::cerr << "Invalid FEN" << std::endl;
        return 1;
    }
    
    auto start = std::chrono::steady_clock::now();
    unsigned long long nodes = 0;
    if (divide && depth > 0) {
        std::vector<Move> moves;
        int color = board.getSideToMove();
        board.getLegalMoves(color, moves);
        for (Move &move : moves) {
            move.doMove(board);
            board.switchSideToMove();
            unsigned long long moveNodes = perft(board, color == 0 ? 1 : 0, depth - 1);
            board.switchSideToMove();
            move.reverseMove(board);
            
            nodes += moveNodes;
            std::cout << formatMove(move) << ": " << moveNodes << std::endl;
        }
    } else {
        nodes = perft(board, board.getSideToMove(), depth);
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double nodesPerSecond = elapsed.count() > 0 ? nodes / elapsed.count() : 0;
    
    std::cout << "Perft depth " << depth << ": " << nodes << " nodes in "
              << elapsed.count() << " seconds (" << nodesPerSecond
              << " nodes/sec)" << std::endl;
    
    return 0;
}

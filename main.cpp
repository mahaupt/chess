#include "board.hpp"
#include "chessAI.hpp"
#include "game.hpp"
#include "move.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


namespace {
const std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";


std::string formatMove(const Move &move)
{
    return move.getFrom().toString() + move.getTo().toString();
}


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
    double nodesPerSecond = elapsed.count() > 0
                            ? ai.getNodesEvaluated() / elapsed.count()
                            : 0;
    
    std::cout << "bestmove " << formatMove(bestMove) << std::endl;
    std::cout << "Computer searched " << ai.getSearchDepth()
              << " plies, evaluated " << ai.getNodesEvaluated()
              << " nodes in " << elapsed.count()
              << " seconds of a " << ai.getTimeBudgetSeconds()
              << " second budget (" << nodesPerSecond << " nodes/sec)" << std::endl;
    
    const ChessAI::TranspositionStats &ttStats = ai.getTranspositionStats();
    std::cout << "Transposition table: " << ttStats.probes << " probes, "
              << ttStats.hits << " hits, "
              << ttStats.exactHits << " exact hits, "
              << ttStats.boundCutoffs << " bound cutoffs, "
              << ttStats.stores << " stores" << std::endl;
    
    return 0;
}
}


int main(int argc, char * argv[]) 
{
    std::string fen;
    std::string perftFEN;
    std::string bestMoveFEN;
    int perftDepth = -1;
    bool perftDivide = false;
    double timeBudgetSeconds = 3.0;
    for (int i = 1; i < argc; i++) {
        std::string argument = argv[i];
        if (argument == "--fen" && i + 1 < argc) {
            fen = argv[++i];
        } else if (argument == "--bestmove" && i + 1 < argc) {
            bestMoveFEN = argv[++i];
        } else if (argument == "--time" && i + 1 < argc) {
            try {
                timeBudgetSeconds = std::stod(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Invalid time budget" << std::endl;
                return 1;
            }
            if (timeBudgetSeconds <= 0) {
                std::cerr << "Invalid time budget" << std::endl;
                return 1;
            }
        } else if ((argument == "--perft" || argument == "--perft-divide") && i + 2 < argc) {
            perftDivide = argument == "--perft-divide";
            perftFEN = argv[++i];
            try {
                perftDepth = std::stoi(argv[++i]);
            } catch (const std::exception&) {
                std::cerr << "Invalid perft depth" << std::endl;
                return 1;
            }
        } else if (argument == "--help" || argument == "-h") {
            std::cout << "Usage: chess [--fen \"<position>\"]" << std::endl;
            std::cout << "       chess --bestmove \"<position|startpos>\" [--time seconds]" << std::endl;
            std::cout << "       chess --perft \"<position|startpos>\" <depth>" << std::endl;
            std::cout << "       chess --perft-divide \"<position|startpos>\" <depth>" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown argument: " << argument << std::endl;
            std::cerr << "Usage: chess [--fen \"<position>\"]" << std::endl;
            std::cerr << "       chess --bestmove \"<position|startpos>\" [--time seconds]" << std::endl;
            std::cerr << "       chess --perft \"<position|startpos>\" <depth>" << std::endl;
            std::cerr << "       chess --perft-divide \"<position|startpos>\" <depth>" << std::endl;
            return 1;
        }
    }
    
    if (!perftFEN.empty()) {
        if (perftDepth < 0) {
            std::cerr << "Invalid perft depth" << std::endl;
            return 1;
        }
        return runPerft(perftFEN, perftDepth, perftDivide);
    }
    
    if (!bestMoveFEN.empty()) {
        return runBestMove(bestMoveFEN, timeBudgetSeconds);
    }
    
    try {
        CGame game(fen);
        while(game.move()) {}
    } catch (const std::runtime_error &error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    
    
    return 0;
}

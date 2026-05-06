#include "benchmark.hpp"
#include "game.hpp"
#include "perft.hpp"
#include "uci.hpp"
#include <iostream>
#include <stdexcept>
#include <string>


int main(int argc, char * argv[]) 
{
    std::string fen;
    std::string perftFEN;
    std::string bestMoveFEN;
    int perftDepth = -1;
    bool perftDivide = false;
    bool uciMode = false;
    double timeBudgetSeconds = 3.0;
    for (int i = 1; i < argc; i++) {
        std::string argument = argv[i];
        if (argument == "--uci") {
            uciMode = true;
        } else if (argument == "--fen" && i + 1 < argc) {
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
            std::cout << "Usage: chess [--fen \"<position>\"] [--time seconds]" << std::endl;
            std::cout << "       chess --bestmove \"<position|startpos>\" [--time seconds]" << std::endl;
            std::cout << "       chess --perft \"<position|startpos>\" <depth>" << std::endl;
            std::cout << "       chess --perft-divide \"<position|startpos>\" <depth>" << std::endl;
            std::cout << "       chess --uci" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown argument: " << argument << std::endl;
            std::cerr << "Usage: chess [--fen \"<position>\"] [--time seconds]" << std::endl;
            std::cerr << "       chess --bestmove \"<position|startpos>\" [--time seconds]" << std::endl;
            std::cerr << "       chess --perft \"<position|startpos>\" <depth>" << std::endl;
            std::cerr << "       chess --perft-divide \"<position|startpos>\" <depth>" << std::endl;
            std::cerr << "       chess --uci" << std::endl;
            return 1;
        }
    }
    
    if (uciMode) {
        return runUci();
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
        CGame game(fen, timeBudgetSeconds);
        while(game.move()) {}
    } catch (const std::runtime_error &error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    
    
    return 0;
}

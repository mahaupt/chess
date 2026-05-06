#include "uci.hpp"

#include "board.hpp"
#include "chessAI.hpp"
#include "chessConstants.hpp"
#include "enpassantflag.hpp"
#include "move.hpp"
#include "moveFormat.hpp"
#include "point.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


namespace {

std::string toLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return value;
}


std::string formatUciMove(const Move &move)
{
    return toLower(formatMove(move));
}


bool parseUciMove(const std::string &text, Move &move)
{
    if (text.size() < 4) {
        return false;
    }
    
    Point from(text.substr(0, 2));
    Point to(text.substr(2, 2));
    if (!from.parsedValid() || !from.isValid() || !to.parsedValid() || !to.isValid()) {
        return false;
    }
    
    move = Move(from, to);
    return true;
}


bool applyUciMove(CBoard &board, EnpassantFlag &enpassantFlag, const std::string &text)
{
    Move requestedMove;
    if (!parseUciMove(text, requestedMove)) {
        return false;
    }
    
    int color = board.getSideToMove();
    std::vector<Move> legalMoves;
    board.getLegalMoves(color, legalMoves, &enpassantFlag);
    
    for (Move &legalMove : legalMoves) {
        if (legalMove.compareTo(requestedMove)) {
            legalMove.doMove(board, enpassantFlag);
            board.switchSideToMove();
            return true;
        }
    }
    
    return false;
}


bool setPosition(CBoard &board, EnpassantFlag &enpassantFlag, const std::vector<std::string> &tokens)
{
    if (tokens.size() < 2) {
        return false;
    }
    
    std::size_t moveStart = 0;
    if (tokens[1] == "startpos") {
        if (!board.loadFEN(startingFEN)) {
            return false;
        }
        moveStart = 2;
    } else if (tokens[1] == "fen") {
        std::size_t fenEnd = 2;
        while (fenEnd < tokens.size() && tokens[fenEnd] != "moves") {
            fenEnd++;
        }
        
        std::ostringstream fen;
        for (std::size_t i = 2; i < fenEnd; i++) {
            if (i > 2) {
                fen << ' ';
            }
            fen << tokens[i];
        }
        
        if (!board.loadFEN(fen.str())) {
            return false;
        }
        moveStart = fenEnd;
    } else {
        return false;
    }
    
    enpassantFlag.resetFlag();
    
    if (moveStart < tokens.size() && tokens[moveStart] == "moves") {
        for (std::size_t i = moveStart + 1; i < tokens.size(); i++) {
            if (!applyUciMove(board, enpassantFlag, tokens[i])) {
                return false;
            }
        }
    }
    
    return true;
}


std::vector<std::string> split(const std::string &line)
{
    std::istringstream input(line);
    std::vector<std::string> tokens;
    std::string token;
    while (input >> token) {
        tokens.push_back(token);
    }
    return tokens;
}


double parseMoveTimeSeconds(const std::vector<std::string> &tokens)
{
    for (std::size_t i = 1; i + 1 < tokens.size(); i++) {
        if (tokens[i] == "movetime") {
            try {
                int milliseconds = std::stoi(tokens[i + 1]);
                if (milliseconds > 0) {
                    return milliseconds / 1000.0;
                }
            } catch (const std::exception&) {
                return 3.0;
            }
        }
    }
    
    return 3.0;
}

}


int runUci()
{
    CBoard board;
    board.loadFEN(startingFEN);
    EnpassantFlag enpassantFlag;
    ChessAI ai;
    
    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<std::string> tokens = split(line);
        if (tokens.empty()) {
            continue;
        }
        
        const std::string &command = tokens[0];
        if (command == "uci") {
            std::cout << "id name chess" << std::endl;
            std::cout << "id author Marcel Haupt" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (command == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (command == "ucinewgame") {
            board.loadFEN(startingFEN);
            enpassantFlag.resetFlag();
            ai = ChessAI();
        } else if (command == "position") {
            if (!setPosition(board, enpassantFlag, tokens)) {
                std::cout << "info string invalid position" << std::endl;
            }
        } else if (command == "go") {
            ai.setTimeBudgetSeconds(parseMoveTimeSeconds(tokens));
            
            std::vector<Move> legalMoves;
            board.getLegalMoves(board.getSideToMove(), legalMoves, &enpassantFlag);
            if (legalMoves.empty()) {
                std::cout << "bestmove 0000" << std::endl;
                continue;
            }
            
            Move bestMove = ai.getNextMove(board, board.getSideToMove());
            std::cout << "bestmove " << formatUciMove(bestMove) << std::endl;
        } else if (command == "quit") {
            break;
        }
    }
    
    return 0;
}

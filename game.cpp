//
//  game.cpp
//  chess
//
//  Created by Marcel Haupt on 08.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#include "game.hpp"
#include <chrono>


CGame::CGame() {
    m_userTurn = 1;

    std::cout << "Which color do you want to play as (B/W): ";
    
    std::string inp_color;
    std::cin >> inp_color;
    
    m_userColor = 0;
    if (inp_color.compare("w") == 0 || inp_color.compare("W") == 0) {
        m_userColor = 1;
    }
    
    //print board for the first time
    m_board.printBoard(m_userColor == 0);
}



bool CGame::move() {
    //display turn color
    if (m_userTurn == 0) {
        std::cout << "Black players turn" << std::endl;
    } else {
        std::cout << "White players turn" << std::endl;
    }
    
    
    
    //players turn
    if (m_userTurn == m_userColor) {
        while(true) {
            std::cout << "Select a figure: ";
    
            std::string inp_figure;
            std::cin >> inp_figure;
            
            Point point1 = Point(inp_figure);
            if (!point1.parsedValid()) {
                std::cout << "Error: invalid input. Example (A5)." << std::endl;
                continue;
            }
            std::cout << "You selected " << point1.toString();
        
            //get figure from board
            CFigure* nfigure = m_board.getFigure(point1);
            if (nfigure == 0) {
                std::cout << std::endl << "There is no figure!" << std::endl;
                continue;
            }
            if (nfigure->getColor() != m_userTurn) {
                std::cout << std::endl << "Invalid figure!" << nfigure->getColor() << std::endl;
                continue;
            }
            std::cout << " - " << nfigure->getFigureSign() << std::endl;
            
            //display possible movements for figure
            std::cout << "Possible target fields: ";
            std::vector< Move > moves = std::vector< Move >();
            m_board.getLegalMoves(point1, m_userTurn, moves, &eflag);
            
            if (moves.size() <= 0) {
                std::cout << "No Fields found!" << std::endl;
                continue;
            }
            
            
            for (int i=0; i < moves.size(); i++) {
                std::cout << moves[i].getTo().toString() << ", ";
            }
            
            
            //get target field
            std::cout << std::endl << "Select target field: ";
            
            std::string inp_field;
            std::cin >> inp_field;
            Point point2 = Point(inp_field);
            if (!point2.parsedValid()) {
                std::cout << "Error: invalid input. Example (A5)." << std::endl;
                continue;
            }
            
            //move figure
            Move move = Move(point1, point2);
            //m_board
            bool validMove = false;
            for (int i=0; i < moves.size(); i++) {
                if (moves[i].compareTo(move)) {
                    validMove = true;
                    move = moves[i];
                    break;
                }
            }
            
            if (validMove) {
                move.doMove(m_board, eflag);
                std::cout << "Move done" << std::endl;
            } else {
                std::cout << "Move invalid" << std::endl;
                continue;
            }
            
            break;
        }
    }
    
    
    //computers turn
    else {
        std::cout << "Calculating..." << std::endl;
        auto start = std::chrono::steady_clock::now();
        Move targetmove = cai.getNextMove(m_board, m_userTurn);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        double nodesPerSecond = elapsed.count() > 0
                                ? cai.getNodesEvaluated() / elapsed.count()
                                : 0;
        std::cout << "Computer searched " << cai.getSearchDepth()
                  << " plies, evaluated " << cai.getNodesEvaluated()
                  << " nodes in " << elapsed.count()
                  << " seconds (" << nodesPerSecond << " nodes/sec)" << std::endl;

        Move stdmove = Move();
        if (targetmove.compareTo(stdmove)) { std::cout << "Error: no move!" << std::endl;return false; }
        
        targetmove.doMove(m_board, eflag);
        std::cout << "Computer moved " << targetmove.getMovedFig()->getFigureSign() << " from " << targetmove.getFrom().toString() << " to " << targetmove.getTo().toString() << std::endl;
    }
    
    
    
    //print board after user turn
    m_board.printBoard(m_userColor == 0);
    
    //other users turn
    m_userTurn = (m_userTurn==0)?1:0;
    
    //check if check mate or remis
    bool check = m_board.isInCheck(m_userTurn);
    bool hasLegalMove = m_board.hasLegalMove(m_userTurn, &eflag);
    std::string playername = (m_userTurn==0) ? "Black" : "White";
    if (check && hasLegalMove) {
        std::cout << playername + " is checked" << std::endl;
    } else if (!hasLegalMove && !check) {
        std::cout << "Game is remis" << std::endl;
        return false;
    } else if (!hasLegalMove && check) {
        std::cout << playername + " is checkmated" << std::endl;
        return false;
    }
    
    return true;
}

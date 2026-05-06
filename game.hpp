//
//  game.hpp
//  chess
//
//  Created by Marcel Haupt on 08.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once
#include <iostream>
#include <string>

#include "board.hpp"
#include "point.hpp"
#include "chessAI.hpp"
#include "enpassantflag.hpp"

class CGame {
public:
    CGame(const std::string &fen = "", double timeBudgetSeconds = 3.0);
    bool move();
    EnpassantFlag eflag;
    
private:
    int m_userColor;
    int m_userTurn;
    CBoard m_board;
    ChessAI cai;
    
};

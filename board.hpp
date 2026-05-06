#pragma once
#include <array>
#include <vector>
#include <iostream>

#include "point.hpp"
#include "move.hpp"
#include "figure.hpp"
#include "figures/pawn.hpp"
#include "figures/king.hpp"
#include "figures/knight.hpp"
#include "figures/queen.hpp"
#include "figures/bishop.hpp"
#include "figures/rook.hpp"


typedef std::array<std::array<CFigure*, 8>, 8> chessboardmap;

class CBoard 
{
    friend class Move;
    friend class EnpassantFlag;
    
private:
    chessboardmap m_board;
    std::vector<CFigure*> m_capturedFigures;
    
    void storeCapturedFigure(CFigure* figure);
    void restoreCapturedFigure(CFigure* figure);
    
protected:
    
public:
    CBoard( void );
    ~CBoard();
    
    void printBoard(bool flipped = false);
    CFigure* getFigure(int x, int y);
    CFigure* getFigure(const Point &pt);
    
    int evaluateBoard(int color);
};

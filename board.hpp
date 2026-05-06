#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

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
class EnpassantFlag;

class CBoard 
{
    friend class Move;
    friend class EnpassantFlag;
    
private:
    chessboardmap m_board;
    std::vector<CFigure*> m_capturedFigures;
    int m_sideToMove;
    
    void storeCapturedFigure(CFigure* figure);
    void restoreCapturedFigure(CFigure* figure);
    void clearBoard();
    void copyFrom(const CBoard & board);
    
protected:
    
public:
    CBoard( void );
    CBoard(const CBoard & board);
    CBoard & operator=(const CBoard & board);
    ~CBoard();
    
    void printBoard(bool flipped = false);
    bool loadFEN(const std::string &fen);
    int getSideToMove() const;
    void setSideToMove(int color);
    void switchSideToMove();
    CFigure* getFigure(int x, int y);
    CFigure* getFigure(const Point &pt);
    
    int evaluateBoard(int color);
    bool isInCheck(int color);
    bool isSquareAttacked(const Point &point, int attackerColor);
    bool hasLegalMove(int color, EnpassantFlag* eflag = 0);
    void getLegalMoves(int color, std::vector<Move> & moves, EnpassantFlag* eflag = 0);
    void getLegalMoves(Point &point, int color, std::vector<Move> & moves, EnpassantFlag* eflag = 0);
    std::uint64_t hash() const;
};

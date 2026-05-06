#include "board.hpp"
#include "enpassantflag.hpp"
#include <cstdlib>
#include <iostream>


namespace {
const char* resetColor = "\033[0m";
const char* lightSquare = "\033[48;5;250m";
const char* darkSquare = "\033[48;5;240m";
const char* whitePiece = "\033[38;5;231m";
const char* blackPiece = "\033[38;5;16m";

void printSquare(CFigure* figure, bool isLightSquare)
{
    std::cout << (isLightSquare ? lightSquare : darkSquare);
    if (figure != 0) {
        std::cout << (figure->getColor() == 1 ? whitePiece : blackPiece);
        std::cout << " " << figure->getFigureSign() << " ";
    } else {
        std::cout << "   ";
    }
    std::cout << resetColor;
}


int getCenterBonus(int x, int y) {
    int fileDistance = std::abs(x * 2 - 7);
    int rankDistance = std::abs(y * 2 - 7);
    return 14 - fileDistance - rankDistance;
}


int getPieceSquareBonus(const CFigure* figure, int x, int y) {
    int forwardRank = figure->getColor() == 1 ? y : 7 - y;
    
    switch (figure->getType()) {
        case FigureType::Pawn:
            return forwardRank * 4 + getCenterBonus(x, y);
        case FigureType::Knight:
            return getCenterBonus(x, y) * 4;
        case FigureType::Bishop:
            return getCenterBonus(x, y) * 2;
        case FigureType::Rook:
            return forwardRank;
        case FigureType::Queen:
            return getCenterBonus(x, y);
        case FigureType::King:
            return 0;
    }
    
    return 0;
}


bool isInsideBoard(int x, int y) {
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}


std::uint64_t mixHash(std::uint64_t value) {
    value ^= value >> 30;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27;
    value *= 0x94d049bb133111ebULL;
    value ^= value >> 31;
    return value;
}
}


CBoard::CBoard( void )
{
    m_sideToMove = 1;
    
	//clear board
	for (int i=0; i<8; i++) 
	{
		for (int j=0; j<8; j++) 
		{
			m_board[i][j] = 0;
		}
	}
    
    
    //#####_SETUP_BOARD_######

    //set pawns
    for (int i = 0; i < 8; i++)
    {
        m_board[i][1] = new CFPawn(1);
        m_board[i][6] = new CFPawn(0);
    }
    
    //set king
    m_board[4][0] = new CFKing(1);
    m_board[4][7] = new CFKing(0);
    
    //set rook
    m_board[0][0] = new CFRook(1);
    m_board[7][0] = new CFRook(1);
    m_board[0][7] = new CFRook(0);
    m_board[7][7] = new CFRook(0);

    //set knight
    m_board[1][0] = new CFKnight(1);
    m_board[6][0] = new CFKnight(1);
    m_board[1][7] = new CFKnight(0);
    m_board[6][7] = new CFKnight(0);

    //set bishop
    m_board[2][0] = new CFBishop(1);
    m_board[5][0] = new CFBishop(1);
    m_board[2][7] = new CFBishop(0);
    m_board[5][7] = new CFBishop(0);

    //set queen
    m_board[3][0] = new CFQueen(1);
    m_board[3][7] = new CFQueen(0);
}


CBoard::CBoard(const CBoard & board)
{
    copyFrom(board);
}


CBoard & CBoard::operator=(const CBoard & board)
{
    if (this != &board) {
        clearBoard();
        copyFrom(board);
    }
    
    return *this;
}




void CBoard::printBoard(bool flipped) {
    const std::string files = flipped ? "H   G   F   E   D   C   B   A"
                                      : "A   B   C   D   E   F   G   H";
    const std::string border = "  +---+---+---+---+---+---+---+---+";

    std::cout << std::endl;
    std::cout << "    " << files << std::endl;
    std::cout << border << std::endl;

    for (int rankIndex = 0; rankIndex < 8; rankIndex++)
    {
        int rank = flipped ? rankIndex : 7 - rankIndex;
        std::cout << rank + 1 << " |";
        for (int fileIndex = 0; fileIndex < 8; fileIndex++)
        {
            int file = flipped ? 7 - fileIndex : fileIndex;
            printSquare(m_board[file][rank], (file + rank) % 2 == 0);
            std::cout << "|";
        }
        std::cout << " " << rank + 1 << std::endl;
        std::cout << border << std::endl;
    }

    std::cout << "    " << files << std::endl;
    std::cout << std::endl;
}


int CBoard::getSideToMove() const {
    return m_sideToMove;
}


void CBoard::setSideToMove(int color) {
    m_sideToMove = color;
}


void CBoard::switchSideToMove() {
    m_sideToMove = m_sideToMove == 0 ? 1 : 0;
}


void CBoard::copyFrom(const CBoard & board) {
    m_sideToMove = board.m_sideToMove;
    
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            CFigure* figure = board.m_board[x][y];
            m_board[x][y] = figure != 0 ? figure->clone() : 0;
        }
    }
}


std::uint64_t CBoard::hash() const {
    std::uint64_t value = mixHash((std::uint64_t)m_sideToMove + 1);
    
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            const CFigure* figure = m_board[x][y];
            if (figure != 0) {
                std::uint64_t pieceValue = (std::uint64_t)figure->getType();
                pieceValue = pieceValue * 2 + (std::uint64_t)figure->getColor();
                pieceValue = pieceValue * 2 + (figure->hasMoved() ? 1ULL : 0ULL);
                pieceValue = pieceValue * 64 + (std::uint64_t)(y * 8 + x);
                value ^= mixHash(pieceValue + 0x9e3779b97f4a7c15ULL);
            }
        }
    }
    
    return value;
}



CFigure* CBoard::getFigure(const Point &pt) {
    return m_board[pt.getX()][pt.getY()];
}

CFigure* CBoard::getFigure(int x, int y) {
    return m_board[x][y];
}


void CBoard::storeCapturedFigure(CFigure* figure) {
    if (figure != 0) {
        m_capturedFigures.push_back(figure);
    }
}


void CBoard::restoreCapturedFigure(CFigure* figure) {
    if (figure == 0 || m_capturedFigures.size() == 0) {
        return;
    }
    
    if (m_capturedFigures.back() == figure) {
        m_capturedFigures.pop_back();
        return;
    }
    
    for (int i = 0; i < m_capturedFigures.size(); i++) {
        if (m_capturedFigures[i] == figure) {
            m_capturedFigures.erase(m_capturedFigures.begin() + i);
            return;
        }
    }
}


bool CBoard::isInCheck(int color) {
    Point kingPoint = Point(-1, -1);
    
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            CFigure* figure = m_board[x][y];
            if (figure != 0 && figure->getColor() == color && figure->getType() == FigureType::King) {
                kingPoint = Point(x, y);
                break;
            }
        }
    }
    
    if (!kingPoint.isValid()) {
        return true;
    }
    
    return isSquareAttacked(kingPoint, color == 0 ? 1 : 0);
}


bool CBoard::isSquareAttacked(const Point &point, int attackerColor) {
    int pawnDirection = attackerColor == 1 ? 1 : -1;
    int pawnY = point.getY() - pawnDirection;
    for (int dx = -1; dx <= 1; dx += 2) {
        int pawnX = point.getX() + dx;
        if (isInsideBoard(pawnX, pawnY)) {
            CFigure* figure = m_board[pawnX][pawnY];
            if (figure != 0 && figure->getColor() == attackerColor && figure->getType() == FigureType::Pawn) {
                return true;
            }
        }
    }
    
    const int knightMoves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {-1, 2}, {1, -2}, {-1, -2}
    };
    for (int i = 0; i < 8; i++) {
        int x = point.getX() + knightMoves[i][0];
        int y = point.getY() + knightMoves[i][1];
        if (isInsideBoard(x, y)) {
            CFigure* figure = m_board[x][y];
            if (figure != 0 && figure->getColor() == attackerColor && figure->getType() == FigureType::Knight) {
                return true;
            }
        }
    }
    
    const int kingMoves[8][2] = {
        {0, 1}, {0, -1}, {1, 1}, {-1, 1},
        {1, -1}, {-1, -1}, {-1, 0}, {1, 0}
    };
    for (int i = 0; i < 8; i++) {
        int x = point.getX() + kingMoves[i][0];
        int y = point.getY() + kingMoves[i][1];
        if (isInsideBoard(x, y)) {
            CFigure* figure = m_board[x][y];
            if (figure != 0 && figure->getColor() == attackerColor && figure->getType() == FigureType::King) {
                return true;
            }
        }
    }
    
    const int straightMoves[4][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}
    };
    for (int i = 0; i < 4; i++) {
        int x = point.getX() + straightMoves[i][0];
        int y = point.getY() + straightMoves[i][1];
        while (isInsideBoard(x, y)) {
            CFigure* figure = m_board[x][y];
            if (figure != 0) {
                if (figure->getColor() == attackerColor
                    && (figure->getType() == FigureType::Rook || figure->getType() == FigureType::Queen)) {
                    return true;
                }
                break;
            }
            x += straightMoves[i][0];
            y += straightMoves[i][1];
        }
    }
    
    const int diagonalMoves[4][2] = {
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };
    for (int i = 0; i < 4; i++) {
        int x = point.getX() + diagonalMoves[i][0];
        int y = point.getY() + diagonalMoves[i][1];
        while (isInsideBoard(x, y)) {
            CFigure* figure = m_board[x][y];
            if (figure != 0) {
                if (figure->getColor() == attackerColor
                    && (figure->getType() == FigureType::Bishop || figure->getType() == FigureType::Queen)) {
                    return true;
                }
                break;
            }
            x += diagonalMoves[i][0];
            y += diagonalMoves[i][1];
        }
    }
    
    return false;
}


void CBoard::getLegalMoves(Point &point, int color, std::vector<Move> & moves, EnpassantFlag* eflag) {
    CFigure* figure = getFigure(point);
    if (figure == 0 || figure->getColor() != color) {
        return;
    }
    
    std::vector<Move> pseudoMoves = std::vector<Move>();
    figure->getMoves(point, *this, pseudoMoves, eflag);
    
    for (int i = 0; i < pseudoMoves.size(); i++) {
        Move move = pseudoMoves[i];
        if (eflag != 0 && move.hitsEnpassant()) {
            move.doMove(*this, *eflag);
            if (!isInCheck(color)) {
                moves.push_back(pseudoMoves[i]);
            }
            move.reverseMove(*this, *eflag);
        } else {
            move.doMove(*this);
            if (!isInCheck(color)) {
                moves.push_back(pseudoMoves[i]);
            }
            move.reverseMove(*this);
        }
    }
}


void CBoard::getLegalMoves(int color, std::vector<Move> & moves, EnpassantFlag* eflag) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Point point = Point(x, y);
            getLegalMoves(point, color, moves, eflag);
        }
    }
}


bool CBoard::hasLegalMove(int color, EnpassantFlag* eflag) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Point point = Point(x, y);
            CFigure* figure = getFigure(point);
            if (figure == 0 || figure->getColor() != color) {
                continue;
            }
            
            std::vector<Move> pseudoMoves = std::vector<Move>();
            figure->getMoves(point, *this, pseudoMoves, eflag);
            
            for (int i = 0; i < pseudoMoves.size(); i++) {
                Move move = pseudoMoves[i];
                bool legal = false;
                if (eflag != 0 && move.hitsEnpassant()) {
                    move.doMove(*this, *eflag);
                    legal = !isInCheck(color);
                    move.reverseMove(*this, *eflag);
                } else {
                    move.doMove(*this);
                    legal = !isInCheck(color);
                    move.reverseMove(*this);
                }
                
                if (legal) {
                    return true;
                }
            }
        }
    }
    
    return false;
}




int CBoard::evaluateBoard(int color) {
    int value = 0;
    
    //figure values
    for (int x=0; x<8; x++)
    {
        for (int y=0; y<8; y++)
        {
            CFigure* figure = m_board[x][y];
            if (figure != 0) {
                int figureValue = figure->getValue() * 100;
                if (figure->getType() != FigureType::King) {
                    figureValue += getPieceSquareBonus(figure, x, y);
                }
                
                if (figure->getColor() == 0) {
                    value += figureValue;
                } else {
                    value -= figureValue;
                }
            }
        }
    }

    
    //return value
    if (color == 1) {
        return -value;
    }
    return value;
}



CBoard::~CBoard( void )
{
    clearBoard();
}


void CBoard::clearBoard()
{
	//erase functions board
	for (int i=0; i<8; i++) 
	{
		for (int j=0; j<8; j++) 
		{
			if (m_board[i][j] != 0) {
                delete m_board[i][j];
                m_board[i][j] = 0;
            }
		}
	}
    
    for (int i = 0; i < m_capturedFigures.size(); i++) {
        delete m_capturedFigures[i];
    }
    m_capturedFigures.clear();
}

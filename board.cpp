#include "board.hpp"


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


CBoard::CBoard( void )
{
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



CFigure* CBoard::getFigure(const Point &pt) {
    return m_board[pt.getX()][pt.getY()];
}

CFigure* CBoard::getFigure(int x, int y) {
    return m_board[x][y];
}




int CBoard::evaluateBoard(int color) {
    int value = 0;
    
    //figure values
    for (int i=0; i<8; i++)
    {
        for (int j=0; j<8; j++)
        {
            if (m_board[i][j] != 0) {
                int figval = m_board[i][j]->getValue();
                if (m_board[i][j]->getColor() == 0) {
                    value += figval * 100;
                    
                    if (figval < 1000)
                        value += j*figval;
                } else {
                    value -= figval * 100;
                    
                    if (figval < 1000)
                        value -= j*figval;
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
}

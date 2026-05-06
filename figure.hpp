#pragma once
#include <vector>
#include <string>

#include "movingPrefab.hpp"

class Move;
class Point;
class CBoard;
class EnpassantFlag;

enum class FigureType {
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};


class CFigure
{
private:
protected:
    int m_color;
    int m_figureValue;
    FigureType m_type;
    bool m_hasMoved = false;
    std::string m_sign;
    std::vector< MovingPrefab > m_movesList;
    
public:
    void getMoves(Point & point, CBoard & board, std::vector< Move > & moves, EnpassantFlag* eflag = 0) const;
    virtual ~CFigure() {}
    virtual CFigure* clone() const = 0;
    
    std::string getFigureSign() const { return m_sign; }
    int getColor() const { return m_color; }
    
    int getValue() const { return m_figureValue; }
    FigureType getType() const { return m_type; }
    bool hasMoved() const { return m_hasMoved; }
    void setHasMoved(bool hasMoved) { m_hasMoved = hasMoved; }
};

#pragma once
#include <vector>
#include <string>

#include "movingPrefab.hpp"

class Move;
class Point;
class CBoard;
class EnpassantFlag;


class CFigure
{
private:
protected:
    int m_color;
    int m_figureValue;
    std::string m_sign;
    std::vector< MovingPrefab > m_movesList;
    
public:
    void getMoves(Point & point, CBoard & board, std::vector< Move > & moves, EnpassantFlag* eflag = 0) const;
    virtual ~CFigure() {}
    
    std::string getFigureSign() { return m_sign; }
    int getColor() { return m_color; }
    
    int getValue() { return m_figureValue; }
};

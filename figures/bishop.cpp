#include "bishop.hpp"
#include "../movingPrefab.hpp"


CFBishop::CFBishop(int color) {
    m_color = color;
    m_figureValue = 3;
    m_type = FigureType::Bishop;
    m_sign = "\u265D";
    
    m_movesList.push_back(MovingPrefab(1, 1, true));
    m_movesList.push_back(MovingPrefab(-1, 1, true));
    m_movesList.push_back(MovingPrefab(1, -1, true));
    m_movesList.push_back(MovingPrefab(-1, -1, true));
}



#include "knight.hpp"
#include "../movingPrefab.hpp"


CFKnight::CFKnight(int color) {
    m_color = color;
    m_figureValue = 3;
    m_sign = "\u265E";

    m_movesList.push_back(MovingPrefab(2, 1));
    m_movesList.push_back(MovingPrefab(2, -1));
    m_movesList.push_back(MovingPrefab(-2, 1));
    m_movesList.push_back(MovingPrefab(-2, -1));
    
    m_movesList.push_back(MovingPrefab(1, 2));
    m_movesList.push_back(MovingPrefab(-1, 2));
    m_movesList.push_back(MovingPrefab(1, -2));
    m_movesList.push_back(MovingPrefab(-1, -2));
    
}


#include "queen.hpp"
#include "../movingPrefab.hpp"


CFQueen::CFQueen(int color) {
    m_color = color;
    m_figureValue = 9;
    m_type = FigureType::Queen;
    m_sign = "\u265B";
    
    m_movesList.push_back(MovingPrefab(0, 1, true));
    m_movesList.push_back(MovingPrefab(0, -1, true));
    m_movesList.push_back(MovingPrefab(1, 1, true));
    m_movesList.push_back(MovingPrefab(-1, 1, true));
    m_movesList.push_back(MovingPrefab(1, -1, true));
    m_movesList.push_back(MovingPrefab(-1, -1, true));
    m_movesList.push_back(MovingPrefab(-1, 0, true));
    m_movesList.push_back(MovingPrefab(1, 0, true));
}


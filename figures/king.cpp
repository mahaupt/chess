#include "king.hpp"
#include "../movingPrefab.hpp"

CFKing::CFKing(int color) {
	this->m_color = color;
    m_figureValue = 10000;
    m_type = FigureType::King;
    this->m_sign = "\u265A";
    
    m_movesList.push_back(MovingPrefab(0, 1));
    m_movesList.push_back(MovingPrefab(0, -1));
    m_movesList.push_back(MovingPrefab(1, 1));
    m_movesList.push_back(MovingPrefab(-1, 1));
    m_movesList.push_back(MovingPrefab(1, -1));
    m_movesList.push_back(MovingPrefab(-1, -1));
    m_movesList.push_back(MovingPrefab(-1, 0));
    m_movesList.push_back(MovingPrefab(1, 0));
    
    //rochade
    if (m_color == 0) {
        m_movesList.push_back(MovingPrefab(2, 0, false, false, false, 4, 7, 7));
        m_movesList.push_back(MovingPrefab(-2, 0, false, false, false, 4, 7, 0));
    } else {
        m_movesList.push_back(MovingPrefab(2, 0, false, false, false, 4, 0, 7));
        m_movesList.push_back(MovingPrefab(-2, 0, false, false, false, 4, 0, 0));
    }
}




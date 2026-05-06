#include "rook.hpp"
#include "../movingPrefab.hpp"



CFRook::CFRook(int color) {
	m_color = color;
    m_figureValue = 5;
    m_type = FigureType::Rook;
    m_sign = "\u265C";

    m_movesList.push_back(MovingPrefab(1, 0, true));
    m_movesList.push_back(MovingPrefab(0, 1, true));
    m_movesList.push_back(MovingPrefab(-1, 0, true));
    m_movesList.push_back(MovingPrefab(0, -1, true));
}



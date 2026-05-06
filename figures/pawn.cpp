#include "pawn.hpp"
#include "../movingPrefab.hpp"


CFPawn::CFPawn(int color) {
	m_color = color;
    m_figureValue = 1;
    m_sign = "\u265F";
    
    if (m_color == 0) {
        m_movesList.push_back(MovingPrefab(0, -1, false, false));
        
        MovingPrefab mph1 = MovingPrefab(1, -1, false, true, true);
        MovingPrefab mph2 = MovingPrefab(-1, -1, false, true, true);
        mph1.setEnpassantHitting();
        mph2.setEnpassantHitting();
        m_movesList.push_back(mph1);
        m_movesList.push_back(mph2);
        
        MovingPrefab mp = MovingPrefab(0, -2, false, false, false, -1, 6);
        mp.setEnpassantMove(0, -1);
        m_movesList.push_back(mp);
    } else {
        m_movesList.push_back(MovingPrefab(0, 1, false, false));
        
        MovingPrefab mph1 = MovingPrefab(1, 1, false, true, true);
        MovingPrefab mph2 = MovingPrefab(-1, 1, false, true, true);
        mph1.setEnpassantHitting();
        mph2.setEnpassantHitting();
        m_movesList.push_back(mph1);
        m_movesList.push_back(mph2);
        
        MovingPrefab mp = MovingPrefab(0, 2, false, false, false, -1, 1);
        mp.setEnpassantMove(0, 1);
        m_movesList.push_back(mp);
    }
}

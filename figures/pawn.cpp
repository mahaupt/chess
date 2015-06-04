#include "pawn.hpp"


CFPawn::CFPawn(void) {
    m_movesList.push_back(CMove(0, 1));
    
}


std::vector<CMove> CFPawn::getMoves(int x, int y) {
    std::vector<CMove> ret;
    ret = m_movesList;

    //pawn at the beginning
    if (y==1) {
        ret.push_back(CMove(0, 2));
    }

    return ret;
}
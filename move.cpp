#include "move.hpp"
#include "board.hpp"
#include "enpassantflag.hpp"


Move::Move(Point _from, Point _to, int _rochadeX) {
    fromfig = 0;
    tofig = 0;
    fromfigHadMoved = false;
    tofigHadMoved = false;
    isRochade = false;
    rochadeX = -1;
    
    from = _from;
    to = _to;
    
    if (_rochadeX >= 0) {
        isRochade = true;
        rochadeX = _rochadeX;
    }
}


Move::Move(): from(Point(0, 0)), to(Point(0,0)) {
    fromfig = 0;
    tofig = 0;
    fromfigHadMoved = false;
    tofigHadMoved = false;
}


const Point & Move::getFrom() const {
    return from;
}


const Point & Move::getTo() const {
    return to;
}


void Move::doMove(CBoard &board)  {
    //rochade move
    if (isRochade) {
        fromfig = board.m_board[from.getX()][from.getY()];
        tofig = board.m_board[rochadeX][from.getY()];
        fromfigHadMoved = fromfig->hasMoved();
        tofigHadMoved = tofig->hasMoved();
        
        board.m_board[from.getX()][from.getY()] = 0;
        board.m_board[rochadeX][from.getY()] = 0;
        
        board.m_board[to.getX()][to.getY()] = fromfig;
        fromfig->setHasMoved(true);
        tofig->setHasMoved(true);
        if (to.getX() > from.getX()) {
            board.m_board[to.getX()-1][to.getY()] = tofig;
        } else {
            board.m_board[to.getX()+1][to.getY()] = tofig;
        }
    }
    
    //other move
    else {
    
        fromfig = board.m_board[from.getX()][from.getY()];
        tofig = board.m_board[to.getX()][to.getY()];
        fromfigHadMoved = fromfig->hasMoved();
        board.storeCapturedFigure(tofig);
    
        board.m_board[from.getX()][from.getY()] = 0;
        board.m_board[to.getX()][to.getY()] = fromfig;
        fromfig->setHasMoved(true);
    }
    
    
}

void Move::doMove(CBoard &board, EnpassantFlag &eflag)  {
    doMove(board);
    
    if (hitEnpassant) {
        eflag.doMove(board);
    } else if (flagEnpassant) {
        eflag.setEnpassantFlag(enpassantPoint, to, fromfig->getColor());
    } else {
        eflag.resetFlag();
    }
}

void Move::reverseMove(CBoard &board, EnpassantFlag &eflag) {
    reverseMove(board);
    
    if (flagEnpassant) {
        eflag.resetFlag();
    }
    if (hitEnpassant) {
        eflag.reverseMove(board);
    }
}


void Move::reverseMove(CBoard &board) {
    //rochade move
    if (isRochade) {
        board.m_board[from.getX()][from.getY()] = fromfig;
        board.m_board[rochadeX][from.getY()] = tofig;
        fromfig->setHasMoved(fromfigHadMoved);
        tofig->setHasMoved(tofigHadMoved);
        
        board.m_board[to.getX()][to.getY()] = 0;
        if (to.getX() > from.getX()) {
            board.m_board[to.getX()-1][to.getY()] = 0;
        } else {
            board.m_board[to.getX()+1][to.getY()] = 0;
        }
    }
    
    //other move
    else {
        board.restoreCapturedFigure(tofig);
        board.m_board[from.getX()][from.getY()] = fromfig;
        board.m_board[to.getX()][to.getY()] = tofig;
        fromfig->setHasMoved(fromfigHadMoved);
    }
    
    fromfig = 0;
    tofig = 0;
    fromfigHadMoved = false;
    tofigHadMoved = false;
}


bool Move::compareTo (Move &move) {
    if (from.getX() == move.getFrom().getX() && from.getY() == move.getFrom().getY()) {
        if (to.getX() == move.getTo().getX() && to.getY() == move.getTo().getY()) {
            return true;
        }
    }
    return false;
}

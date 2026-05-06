#include "move.hpp"
#include "board.hpp"
#include "enpassantflag.hpp"


Move::Move(Point _from, Point _to, int _rochadeX) {
    fromfig = 0;
    tofig = 0;
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
}


const Point & Move::getFrom() const {
    return from;
}


const Point & Move::getTo() const {
    return to;
}


void Move::doMove(CBoard &board)  {
    deleteFigs();
    
    //rochade move
    if (isRochade) {
        fromfig = board.m_board[from.getX()][from.getY()];
        tofig = board.m_board[rochadeX][from.getY()];
        
        board.m_board[from.getX()][from.getY()] = 0;
        board.m_board[rochadeX][from.getY()] = 0;
        
        board.m_board[to.getX()][to.getY()] = fromfig;
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
    
        board.m_board[from.getX()][from.getY()] = 0;
        board.m_board[to.getX()][to.getY()] = fromfig;
    }
    
    
}

void Move::doMove(CBoard &board, EnpassantFlag &eflag)  {
    doMove(board);
    
    if (flagEnpassant) {
        eflag.setEnpassantFlag(enpassantPoint, to);
    }
    if (hitEnpassant) {
        eflag.doMove(board);
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
        
        board.m_board[to.getX()][to.getY()] = 0;
        if (to.getX() > from.getX()) {
            board.m_board[to.getX()-1][to.getY()] = 0;
        } else {
            board.m_board[to.getX()+1][to.getY()] = 0;
        }
    }
    
    //other move
    else {
        board.m_board[from.getX()][from.getY()] = fromfig;
        board.m_board[to.getX()][to.getY()] = tofig;
    }
    
    fromfig = 0;
    tofig = 0;
}


void Move::deleteFigs() {
    /*if (fromfig != 0) {
        delete fromfig;
    }*/
    if (tofig != 0 && !isRochade) {
        delete tofig;
    }
}

bool Move::compareTo (Move &move) {
    if (from.getX() == move.getFrom().getX() && from.getY() == move.getFrom().getY()) {
        if (to.getX() == move.getTo().getX() && to.getY() == move.getTo().getY()) {
            return true;
        }
    }
    return false;
}

Move::~Move() {
    deleteFigs();
}

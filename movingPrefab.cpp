//
//  movingPrefab.cpp
//  chess
//
//  Created by Marcel Haupt on 09.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#include "movingPrefab.hpp"
#include "enpassantflag.hpp"



MovingPrefab::MovingPrefab(int _dx, int _dy, bool _continuous, bool _allowBeat, bool _requireBeat, int _reqX, int _reqY, int _rochadeRookX) {
    dx = _dx;
    dy = _dy;
    
    continuous = _continuous;
    allowBeat = _allowBeat;
    requireBeat = _requireBeat;
    requireXPos = false;
    requireYPos = false;
    isRochade = false;
    rochadeRookX = -1;
    reqX = -1;
    reqY = -1;
    
    if (_reqX >= 0) {
        reqX = _reqX;
        requireXPos = true;
    }
    
    if (_reqY >= 0) {
        reqY = _reqY;
        requireYPos = true;
    }
    
    if (_rochadeRookX >= 0) {
        rochadeRookX = _rochadeRookX;
        isRochade = true;
    }
}


void MovingPrefab::getMoves(Point & point, CBoard & board, std::vector< Move > & moves, EnpassantFlag* eflag) const {
    CFigure * figure = board.getFigure(point);
    
    for (int i=1; i<=8; i++) {
        Point pt = Point(point.getX() + i*dx, point.getY() + i*dy);
        if (!pt.isValid()) break;
        
        
        if ((requireXPos && point.getX() != reqX) || (requireYPos && point.getY() != reqY)) {
            break;
        }
        
        CFigure * endfig = board.getFigure(pt);
        if (endfig != 0 && figure != 0) {
            if (endfig->getColor() == figure->getColor()) {
                break;
            }
        }
        
        //rochade
        if (isRochade) {
            Point rookpt = Point(rochadeRookX, point.getY());
            CFigure * rookfig = board.getFigure(rookpt);
            if (rookfig != 0) {
                if (figure == 0
                    || figure->hasMoved()
                    || rookfig->hasMoved()
                    || rookfig->getColor() != figure->getColor()
                    || rookfig->getType() != FigureType::Rook) {
                    break;
                }
                if (board.isInCheck(figure->getColor())) {
                    break;
                }
                
                int step = rochadeRookX > point.getX() ? 1 : -1;
                Point throughPoint = Point(point.getX() + step, point.getY());
                int attackerColor = figure->getColor() == 0 ? 1 : 0;
                if (board.isSquareAttacked(throughPoint, attackerColor)
                    || board.isSquareAttacked(pt, attackerColor)) {
                    break;
                }
                
                //check space in between
                if (rochadeRookX > point.getX()) {
                    if (board.getFigure(point.getX()+1, point.getY()) != 0)
                        break;
                    if (board.getFigure(point.getX()+2, point.getY()) != 0)
                        break;
                } else {
                    if (board.getFigure(point.getX()-1, point.getY()) != 0)
                        break;
                    if (board.getFigure(point.getX()-2, point.getY()) != 0)
                        break;
                    if (board.getFigure(point.getX()-3, point.getY()) != 0)
                        break;
                }
            } else {
                break;
            }
        }
        
        bool pushbackMove = false;
        
        if (endfig != 0) { // there is figure to beat
            if (allowBeat) {
                pushbackMove = true;
            }
        } else { // no figure to beat
            if (!requireBeat) {
                pushbackMove = true;
            }
        }
        
        
        if (pushbackMove) {
            if (setEnpassantFlag) {
                Move mv = Move(point, pt, rochadeRookX);
                Point epassantpt = Point(point.getX() + enpassantDx, point.getY() + enpassantDy);
                mv.setEnpassantFlag(epassantpt);
                moves.push_back(mv);
            } else {
                moves.push_back(Move(point, pt, rochadeRookX));
            }
            
            if (endfig != 0) {
                break;
            }
        } else {
        
        
            //enpassant
            if (eflag != 0 && enableEnpassantHitting && endfig == 0) {
                if (eflag->getFlag()
                    && eflag->getFigureColor() != figure->getColor()
                    && eflag->getHitPoint().compateTo(pt)) {
                    Move mv = Move(point, pt, rochadeRookX);
                    mv.setHitEnpassant();
                    moves.push_back(mv);
                }
            }
        }
        
        
        
        
        if (!continuous) break;
    }
}




void MovingPrefab::setEnpassantMove(int _dx, int _dy) {
    setEnpassantFlag = true;
    enpassantDx = _dx;
    enpassantDy = _dy;
}




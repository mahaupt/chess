//
//  enpassantflag.hpp
//  chess
//
//  Created by Marcel Haupt on 13.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once

class CFigure;
#include "point.hpp"
#include "board.hpp"



class EnpassantFlag {
private:
    Point hitpoint;
    Point figurepoint;
    CFigure * figure = 0;
    int figureColor = -1;
    bool enpassantFlag = false;
    
public:
    void setEnpassantFlag(Point &hitpoint, Point & figurepoint, int color);
    void doMove(CBoard & board);
    void reverseMove(CBoard & board);
    bool getFlag() { return enpassantFlag; }
    void resetFlag();
    int getFigureColor() { return figureColor; }
    
    const Point & getHitPoint() const { return hitpoint; }
};

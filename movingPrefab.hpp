//
//  movingPrefab.hpp
//  chess
//
//  Created by Marcel Haupt on 09.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#pragma once
#include <vector>

class EnpassantFlag;
class Point;
class Move;
class CBoard;


class MovingPrefab {
private:
    int dx, dy;
    
    bool continuous;
    bool allowBeat;
    bool requireBeat;
    
    bool requireXPos;
    bool requireYPos;
    int reqX;
    int reqY;
    
    bool isRochade;
    int rochadeRookX;
    
    //enpassant
    bool setEnpassantFlag = false;
    int enpassantDx;
    int enpassantDy;
    
    bool enableEnpassantHitting = false;
public:
    MovingPrefab(int _dx, int _dy, bool _continuous = false, bool _allowBeat = true, bool _requireBeat = false, int _reqX = -1, int _reqY = -1,  int _rochadeRookX = -1);
    
    void setEnpassantMove(int _dx, int _dy);
    void setEnpassantHitting() { enableEnpassantHitting = true; }
    
    void getMoves(Point & point, CBoard & board, std::vector< Move > & moves, EnpassantFlag* eflag = 0) const;
};

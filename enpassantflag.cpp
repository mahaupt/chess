//
//  enpassantflag.cpp
//  chess
//
//  Created by Marcel Haupt on 13.12.15.
//  Copyright © 2015 Marcel Haupt. All rights reserved.
//

#include "enpassantflag.hpp"
#include "figure.hpp"


void EnpassantFlag::setEnpassantFlag(Point &_hitpoint, Point & _figurepoint, int color) {
    hitpoint = _hitpoint;
    figurepoint = _figurepoint;
    figureColor = color;
    enpassantFlag = true;
}


void EnpassantFlag::resetFlag() {
    enpassantFlag = false;
    figureColor = -1;
}

void EnpassantFlag::doMove(CBoard & board) {
    figure = board.m_board[figurepoint.getX()][figurepoint.getY()];
    board.storeCapturedFigure(figure);
    board.m_board[figurepoint.getX()][figurepoint.getY()] = 0;
    enpassantFlag = false;
}


void EnpassantFlag::reverseMove(CBoard &board) {
    board.restoreCapturedFigure(figure);
    board.m_board[figurepoint.getX()][figurepoint.getY()] = figure;
    figureColor = figure->getColor();
    figure = 0;
    enpassantFlag = true;
}

#pragma once

#pragma once
#include <vector>

#include "../figure.hpp"
#include "../board.hpp"
#include "../movingPrefab.hpp"
#include "../move.hpp"

class CFRook : public CFigure
{
private:
protected:
public:
    CFRook( int color );
    ~CFRook() {}
    CFigure* clone() const override { return new CFRook(*this); }
};

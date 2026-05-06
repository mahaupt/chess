#pragma once


#include "../figure.hpp"


class CFBishop : public CFigure
{
private:
protected:
public:
    CFBishop( int color );
    ~CFBishop() {}
    CFigure* clone() const override { return new CFBishop(*this); }
};

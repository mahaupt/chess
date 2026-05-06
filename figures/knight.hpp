#pragma once
#include "../figure.hpp"


class CFKnight : public CFigure
{
private:
protected:
public:
    CFKnight( int color );
    ~CFKnight() {}
    CFigure* clone() const override { return new CFKnight(*this); }
};

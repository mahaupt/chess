#pragma once


#include "../figure.hpp"


class CFPawn : public CFigure
{
private:
protected:
public:
    CFPawn( int color );
    ~CFPawn() {}
    CFigure* clone() const override { return new CFPawn(*this); }
};

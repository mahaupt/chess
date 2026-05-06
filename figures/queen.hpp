#pragma once

#include "../figure.hpp"


class CFQueen : public CFigure
{
private:
protected:
public:
    CFQueen( int color );
    ~CFQueen() {}
    CFigure* clone() const override { return new CFQueen(*this); }
};


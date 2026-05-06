#pragma once

#pragma once
#include <vector>

#include "../figure.hpp"


class CFKing : public CFigure
{
private:
protected:
public:
    CFKing( int color );
    ~CFKing() {}
    CFigure* clone() const override { return new CFKing(*this); }
};

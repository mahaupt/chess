#include "moveFormat.hpp"

#include "move.hpp"


std::string formatMove(const Move &move)
{
    return move.getFrom().toString() + move.getTo().toString();
}

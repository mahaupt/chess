#pragma once

#include <iosfwd>

class ChessAI;

void printSearchStats(const ChessAI &ai, double elapsedSeconds, std::ostream &output);

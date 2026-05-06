#pragma once

#include <string>

class CBoard;

unsigned long long perft(CBoard &board, int color, int depth);
int runPerft(const std::string &fen, int depth, bool divide);

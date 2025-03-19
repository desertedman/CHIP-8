#pragma once

#ifndef COLUMNS
#define COLUMNS 64
#endif

#ifndef ROWS
#define ROWS 32
#endif

#include <array>

class Graphics
{
private:
    std::array<std::array<bool, COLUMNS>, ROWS> mScreen; // 32 rows of 64 columns

public:
    Graphics();
    void clearScreen();
    void fillScreen();
    void printScreen();
};
#pragma once

#ifndef COLUMNS
#define COLUMNS 64
#endif

#ifndef ROWS
#define ROWS 32
#endif

#include <array>

class GPU
{
private:
    std::array<std::array<bool, COLUMNS>, ROWS> mScreen; // 32 rows of 64 columns

public:
    void initialize();
    void clearScreen();
    void fillScreen();
    void drawScreen();

    bool getPixel(int x, int y);
    void setPixel(int x, int y, bool status);
};
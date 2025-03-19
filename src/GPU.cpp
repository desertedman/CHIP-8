#include "GPU.h"

#include <iostream>

GPU::GPU()
{
    clearScreen(); // Ensure that screen is cleared on creation. Probably unnecessary
}

void GPU::clearScreen()
{
    for (int x = 0; x < ROWS; x++)
    {
        for (int y = 0; y < COLUMNS; y++)
        {
            mScreen.at(x).at(y) = false;
        }
    }
}

void GPU::fillScreen()
{
    for (int x = 0; x < ROWS; x++)
    {
        for (int y = 0; y < COLUMNS; y++)
        {
            mScreen.at(x).at(y) = true;
        }
    }
}

void GPU::drawScreen()
{
    for (int x = 0; x < ROWS; x++)
    {
        for (int y = 0; y < COLUMNS; y++)
        {
            if (mScreen.at(x).at(y))
            {
                std::cout << "1";
            }

            else
            {
                std::cout << "0";
            }
        }

        std::cout << std::endl; // End of row; start new line;
    }
}

#include "GPU.h"

#include <iostream>

void GPU::initialize()
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
    // Draw top border
    for (int i = 0; i < COLUMNS; i++)
    {
        std::cout << "_";
    }
    std::cout << std::endl;

    for (int x = 0; x < ROWS; x++)
    {
        std::cout << "| "; // Beginning of screen row

        for (int y = 0; y < COLUMNS; y++)
        {
            if (mScreen.at(x).at(y))
            {
                std::cout << "*";
            }

            else
            {
                std::cout << " ";
            }
        }

        std::cout << " |" << std::endl; // End of row; start new line;
    }

    // Draw bottom border
    for (int i = 0; i < COLUMNS; i++)
    {
        std::cout << "_";
    }
    std::cout << std::endl;
}

bool GPU::getPixel(int x, int y) // Ex. 64 x 32
{
    // Traverse y Vector first, then x Vector to get X,Y coord
    if (mScreen.at(y).at(x))
    {
        return true;
    }

    return false;
}

void GPU::setPixel(int x, int y, bool status)
{
    // Traverse y Vector first, then x Vector to get X,Y coord
    mScreen.at(y).at(x) = status;
}

void GPU::xorPixel(int x, int y, bool status)
{
    bool *currPixel = &mScreen.at(y).at(x);

    // XOR operation
    *currPixel ^= 1;
}


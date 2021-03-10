// celularAutamata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
using namespace std;

#include "olcConsoleGameEngine.h"

class gameOfLife : public olcConsoleGameEngine {
public:
    gameOfLife()
    {
        // Name your application
        m_sAppName = L"Game of Life";
    }

private:
    int* m_output;
    int* m_state;

    int playerPositionX;
    int playerPositionY;
    int playerSize;

public:
    bool OnUserCreate() override
    {
        srand(time(0));
        m_output = new int[ScreenWidth() * ScreenHeight()];
        m_state = new int[ScreenWidth() * ScreenHeight()];
        memset(m_output, 0, ScreenWidth() * ScreenHeight() * sizeof(int));
        memset(m_state, 0, ScreenWidth() * ScreenHeight() * sizeof(int));

        for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
            m_state[i] = rand() % 2;

        playerPositionX = ScreenWidth() / 2;
        playerPositionY = ScreenHeight() / 2;
        playerSize = 1;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        auto cell = [&](int x, int y)
        {
            return m_output[y * ScreenWidth() + x];
        };

        for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
        {
            m_output[i] = m_state[i];
        }

        m_output[playerPositionX + playerPositionY * ScreenHeight()] = 1;

        for (int x = 1; x < ScreenWidth() - 1; x++)
            for (int y = 1; y < ScreenHeight() - 1; y++)
            {
                int nNeighbours = cell(x - 1, y - 1) + cell(x    , y - 1) + cell(x + 1, y - 1) +
                                  cell(x - 1, y    ) +                      cell(x + 1, y    ) +
                                  cell(x - 1, y + 1) + cell(x    , y + 1) + cell(x + 1, y + 1);

                if (cell(x, y) == 1)
                    m_state[y * ScreenWidth() + x]= nNeighbours == 2 || nNeighbours == 3;
                else
                    m_state[y * ScreenWidth() + x] = nNeighbours == 3;

                if (cell(x, y) == 1)
                    Draw(x, y, PIXEL_SOLID, FG_WHITE);
                else
                    Draw(x, y, PIXEL_SOLID, FG_BLACK);
            }

        if (GetKey(VK_DOWN).bHeld)
            playerPositionY++;
        if (GetKey(VK_UP).bHeld)
            playerPositionY--;
        if (GetKey(VK_RIGHT).bHeld)
            playerPositionX++;
        if (GetKey(VK_LEFT).bHeld)
            playerPositionX--;

        if (GetKey(VK_LSHIFT).bHeld)
            playerSize++;
        if (GetKey(VK_LCONTROL).bHeld)
            playerSize--;

        if (playerSize > 1)
        {
            for (int x = 0; x <= playerSize; x++)
                for (int y = 0; y <= playerSize; y++)
                {
                    m_state[playerPositionX + x + (playerPositionY - y) * ScreenHeight()] = 1;
                    m_state[playerPositionX - x + (playerPositionY - y) * ScreenHeight()] = 1;
                    m_state[playerPositionX + x + (playerPositionY + y) * ScreenHeight()] = 1;
                    m_state[playerPositionX - x + (playerPositionY + y) * ScreenHeight()] = 1;

                    Draw(playerPositionX + x, playerPositionY + y, PIXEL_SOLID, FG_MAGENTA);
                    Draw(playerPositionX + x, playerPositionY - y, PIXEL_SOLID, FG_MAGENTA);
                    Draw(playerPositionX - x, playerPositionY + y, PIXEL_SOLID, FG_MAGENTA);
                    Draw(playerPositionX - x, playerPositionY - y, PIXEL_SOLID, FG_MAGENTA);
                }
        }
        else
        {
            m_state[playerPositionX + playerPositionY * ScreenHeight()] = 1;
            Draw(playerPositionX, playerPositionY, PIXEL_SOLID, FG_MAGENTA);
        }
        
        
        return true;
    }
};

int main()
{
    gameOfLife game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();
}
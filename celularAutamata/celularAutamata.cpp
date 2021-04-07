// celularAutamata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
using namespace std;

// Main game engine for this project
#include "olcConsoleGameEngine.h"

// Passes before delayed auto-shift kicks in
#ifndef DAS_TIME
#define DAS_TIME 15
#endif // !DAS_TIME


class gameOfLife : public olcConsoleGameEngine {
public:
    gameOfLife()
    {
        // Name your application
        m_sAppName = L"Game of Life";
    }

private:
    // Screen display
    int* m_output;

    // State of each life in a pass
    int* m_state;

    // The color of each life
    int* m_color;

    // Non UI game height
    int gameHeight;

    // UI height
    int UIHeight;

    // Current fittest color for evolution
    COLOUR _fittest_color;

    // Timer for random fittest color
    int _fittest_color_timer;

    // Deisgn for cover
    wstring GOL_logo;
    
    // Player position
    int playerPositionX;
    int playerPositionY;

    // Seed locations
    int* m_seedLocations;

    // Possible seed colors
    COLOUR* m_colors;

    // Possible fit colors
    COLOUR* m_fit_colors;

    // Map for how colors are mixed
    map<pair<COLOUR, COLOUR>, COLOUR> color_mixing_map;

    // Game state
    enum GameState 
    {
        INSTRUCTION = 0,
        SEEDING,
        LIFE,
        EXTINCTION
    };

    // Seed color options
    enum ColorPlacement
    {
        RED = 0,
        GREEN,
        BLUE,
        RANDOM,
        NUM_COLOR_SELECTION
    };

    // Fit color options
    enum FitColor
    {
        FIT_RED = 0,
        FIT_GREEN,
        FIT_BLUE,
        FIT_CYAN,
        FIT_MAGENTA,
        FIT_YELLOW,
        FIT_RANDOM,
        NUM_FIT_COLOR
    };

    // Current Game state
    GameState currentState;

    // Seed color selection
    ColorPlacement selectedColor;

    // Fit color selection
    FitColor selectedFitColor;

public:
    bool OnUserCreate() override
    {
        // Seed rand
        srand(time(0));

        // Set UI height
        UIHeight = 5;

        // Initial seed selected color
        selectedColor = RANDOM;

        // Initial fit selected color
        selectedFitColor = FIT_RANDOM;

        // Set fittest random timer
        _fittest_color_timer = 0;

        // Set game height
        gameHeight = ScreenHeight() - UIHeight;
        
        // Set game output
        m_output = new int[ScreenWidth() * gameHeight];
        
        // Set initial states
        m_state = new int[ScreenWidth() * gameHeight];
        
        // Set initial seed locations
        m_seedLocations = new int[ScreenWidth() * gameHeight];
        
        // Set initial colors
        m_color = new int[ScreenWidth() * gameHeight];
        
        // Set possible seed colors
        m_colors = new COLOUR[]{ FG_RED, FG_GREEN, FG_BLUE};
        
        // Set possible fit colors
        m_fit_colors = new COLOUR[]{ FG_RED, FG_GREEN, FG_BLUE,FG_CYAN,FG_MAGENTA,FG_YELLOW};
        
        // Pick a random initial fit color
        _fittest_color = m_fit_colors[rand() % 6];

        // Populate color mixing map
        pair<COLOUR, COLOUR> col_pair = { FG_RED, FG_GREEN };
        color_mixing_map.emplace(col_pair, FG_YELLOW);
        col_pair = { FG_GREEN, FG_RED };
        color_mixing_map.emplace(col_pair, FG_YELLOW);
        col_pair = { FG_RED, FG_RED };
        color_mixing_map.emplace(col_pair, FG_RED);
        col_pair = { FG_GREEN, FG_GREEN };
        color_mixing_map.emplace(col_pair, FG_GREEN);
        col_pair = { FG_BLUE, FG_GREEN };
        color_mixing_map.emplace(col_pair, FG_CYAN);
        col_pair = { FG_GREEN, FG_BLUE };
        color_mixing_map.emplace(col_pair, FG_CYAN);
        col_pair = { FG_RED, FG_BLUE };
        color_mixing_map.emplace(col_pair, FG_MAGENTA);
        col_pair = { FG_BLUE, FG_RED };
        color_mixing_map.emplace(col_pair, FG_MAGENTA);
        col_pair = { FG_BLUE, FG_BLUE };
        color_mixing_map.emplace(col_pair, FG_BLUE);

        // Set main menu page
        GOL_logo.append(L" XXX   X  X   X XXX   XXXX XXX   X   XXX XXX XXX");
        GOL_logo.append(L"X     X X XX XX X     X  X X     X    X  X   X  ");
        GOL_logo.append(L"X  XX XXX X X X XXX   X  X XXX   X    X  XXX XXX");
        GOL_logo.append(L"X   X X X X   X X     X  X X     X    X  X   X  ");
        GOL_logo.append(L" XXX  X X X   X XXX   XXXX X     XXX XXX X   XXX");
        GOL_logo.append(L"================================================");
        GOL_logo.append(L"                 INSTRUCTIONS                   ");
        GOL_logo.append(L"                                                ");
        GOL_logo.append(L"    While Seeding:                              ");
        GOL_logo.append(L"    WASD/ARROW KEYS: move cursor                ");
        GOL_logo.append(L"    SPACE          : place seed                 ");
        GOL_logo.append(L"    ENTER          : proceed to life            ");
        GOL_logo.append(L"    TAB            : change seeding color       ");
        GOL_logo.append(L"                                                ");
        GOL_logo.append(L"    Anytime:                                    ");
        GOL_logo.append(L"    F5             : back to main menu          ");
        GOL_logo.append(L"    Left Shift     : increase framerate         ");
        GOL_logo.append(L"    Left Control   : decrease framerate         ");
        GOL_logo.append(L"    ESC            : quit                       ");
        GOL_logo.append(L"                                                ");
        GOL_logo.append(L"    RULES OF LIFE:                              ");
        GOL_logo.append(L"    1. When next to less than 2 life            ");
        GOL_logo.append(L"       dies of lonelyness.                      ");
        GOL_logo.append(L"    2. When next to more than 3 life            ");
        GOL_logo.append(L"       dies of overcrowding.                    ");
        GOL_logo.append(L"    3. When a dead cell has exactly 3           ");
        GOL_logo.append(L"       life next to it a new life is created    ");
        GOL_logo.append(L"       at that location.                        ");
        GOL_logo.append(L"                                                ");
        GOL_logo.append(L"    INHERITANCE:                                ");
        GOL_logo.append(L"    When a new life is made 2 of the cell are   ");
        GOL_logo.append(L"    chosen at random to be parents and the      ");
        GOL_logo.append(L"    the color of the new life is determined     ");
        GOL_logo.append(L"    via inheritance(think punnet squares) from  ");
        GOL_logo.append(L"    the parents. All colors are co-dominant.    ");
        GOL_logo.append(L"    There is a 1% chance at gene mutation.      ");
        GOL_logo.append(L"                                                ");
        GOL_logo.append(L"  **PRESS SPACE TO CONTINUE TO SEEDING**        ");

        // Reset arrays
        memset(m_output, 0, ScreenWidth() * gameHeight * sizeof(int));
        memset(m_state, 0, ScreenWidth() * gameHeight * sizeof(int));
        memset(m_seedLocations, 0, ScreenWidth() * gameHeight * sizeof(int));
        memset(m_color, 0, ScreenWidth() * gameHeight * sizeof(int));

        // Set player position
        playerPositionX = ScreenWidth() / 2;
        playerPositionY = gameHeight / 2;

        // Set initial game state
        currentState = GameState::INSTRUCTION;

        return true;

    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Time between frames
        static auto sleepTime = 20ms;

        // Sleep thread for allotted time
        this_thread::sleep_for(sleepTime);

        // Reset screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), 9608, 0);
        
        // Draw UI seperator
        DrawLine(0, gameHeight, ScreenWidth(), gameHeight,61);
        
        // Main menu
        if (currentState == GameState::INSTRUCTION)
        {
            // Draw the main menu
            DrawChars(ScreenWidth() / 2 - 48 / 2, 10, 48, 38, GOL_logo);
            
            // space to go to seeding
            if (GetKey(VK_SPACE).bReleased)
                currentState = GameState::SEEDING;
        }

        // When seeding
        if (currentState == GameState::SEEDING)
        {
            // Display seed color
            wstring seeding = L"Seed Color(TAB to change): ";
            DrawString(2, gameHeight + 1, seeding);
            wstring seeding_color = L"";
            switch (selectedColor)
            {
                case RED:
                    DrawString(29, gameHeight + 1, L"RED", FG_RED);
                    break;
                case GREEN:
                    DrawString(29, gameHeight + 1, L"GREEN", FG_GREEN);
                    break;
                case BLUE:
                    DrawString(29, gameHeight + 1, L"BLUE", FG_BLUE);
                    break;
                case RANDOM:
                    wstring rand_str = L"RANDOM";
                    static int colorAnim = 0;
                    
                    // Alternate colors
                    for (int i = 0; i < rand_str.size(); i++)
                    {
                        wstring rand_char_to_str = L"";
                        rand_char_to_str.push_back(rand_str[i]);
                        DrawString(29 + i, gameHeight + 1, rand_char_to_str, m_colors[(colorAnim / 10 + i) % 3]);
                    }
                    colorAnim++;
                        break;
            }

            // DAS(delayed auto-shift)
            static int das = 0;

            // Move with 1 press in a direction 
            // WASD and arrow keys
            if (GetKey(VK_DOWN).bPressed ||
                GetKey(0x53).bPressed)
                playerPositionY++;
            if (GetKey(VK_UP).bPressed ||
                GetKey(0x57).bPressed)
                playerPositionY--;
            if (GetKey(VK_RIGHT).bPressed ||
                GetKey(0x44).bPressed)
                playerPositionX++;
            if (GetKey(VK_LEFT).bPressed ||
                GetKey(0x41).bPressed)
                playerPositionX--;

            // Check held for DAS
            if (GetKey(VK_DOWN).bHeld ||
                GetKey(VK_UP).bHeld ||
                GetKey(VK_LEFT).bHeld ||
                GetKey(VK_RIGHT).bHeld ||
                GetKey(0x53).bHeld ||
                GetKey(0x57).bHeld ||
                GetKey(0x44).bHeld ||
                GetKey(0x41).bHeld)
            {
                if (das < DAS_TIME)
                    das++;
            }
            else if (das > 0)
                das = 0;
            
            // Move in a directin while held and DAS active
            if (GetKey(VK_DOWN).bHeld ||
                GetKey(0x53).bHeld)
            {
                if (das >= DAS_TIME)
                    playerPositionY++;
            }
            else if (GetKey(VK_UP).bHeld ||
                     GetKey(0x57).bHeld)
            {
                if (das >= DAS_TIME)
                    playerPositionY--;
            }
            if (GetKey(VK_RIGHT).bHeld ||
                GetKey(0x44).bHeld)
            {
                if (das >= DAS_TIME)
                    playerPositionX++;
            }
            else if (GetKey(VK_LEFT).bHeld ||
                     GetKey(0x41).bHeld)
            {
                if (das >= DAS_TIME)
                    playerPositionX--;
            }

            // Swap seed color
            if (GetKey(VK_TAB).bPressed)
                selectedColor = (ColorPlacement)((selectedColor + 1) % NUM_COLOR_SELECTION);
            
            // Clamp to play area
            while (playerPositionY > gameHeight)
            {
                playerPositionY -= gameHeight;
            }
            while (playerPositionY < 0)
            {
                playerPositionY += gameHeight;
            }

            // Draw seeded locations
            for (int x = 0; x < ScreenWidth(); x++)
                for (int y = 0; y < gameHeight; y++)
                {
                    if(m_seedLocations[x + ScreenWidth() * y] == 1)
                        Draw(x, y, PIXEL_SOLID, m_color[x + y * ScreenWidth()]);
                    else
                        Draw(x, y, PIXEL_SOLID, FG_BLACK);
                }
                
            // Draw player
            Draw(playerPositionX, playerPositionY, PIXEL_SOLID, FG_MAGENTA);

            // Space to place a seed
            // Remove seed if it already exists
            if (GetKey(VK_SPACE).bPressed)
            {
                if (m_seedLocations[playerPositionX + playerPositionY * ScreenWidth()] == 0)
                {
                    m_seedLocations[playerPositionX + playerPositionY * ScreenWidth()] = 1;
                    m_color[playerPositionX + playerPositionY * ScreenWidth()] = selectedColor == RANDOM ? m_colors[rand() % 3] : m_colors[selectedColor];
                }
                else
                {
                    m_seedLocations[playerPositionX + playerPositionY * ScreenWidth()] = 0;
                    m_color[playerPositionX + playerPositionY * ScreenWidth()] = FG_BLACK;
                }
            }

            // Enter to go to life stage
            if (GetKey(VK_RETURN).bPressed)
            {
                currentState = GameState::LIFE;

                for (int i = 0; i < gameHeight * ScreenWidth(); i++)
                {
                    m_state[i] = m_seedLocations[i];
                }
            }
        }
        
        // Life is happening
        if (currentState == GameState::LIFE)
        {
            // Num life count
            int numLife = 0;

            // Lambda to get the status of a cell
            auto cell = [&](int x, int y)
            {
                // Clamp to play area
                if (x < 0)
                    x += ScreenWidth();

                // Clamp to play area
                if (y < 0)
                    y += gameHeight;

                // Return value of cell
                return m_output[(y % gameHeight) * ScreenWidth() + (x % ScreenWidth())];
            };

            // Lambda to get the cell color
            auto cell_color = [&](int x, int y)
            {
                // Clamp to play area
                if (x < 0)
                    x += ScreenWidth();

                // Clamp to play area
                if (y < 0)
                    y += gameHeight;
                
                // Return cell color
                return (COLOUR)m_color[(y % gameHeight) * ScreenWidth() + (x % ScreenWidth())];
            };

            // Lambda to get the source colors of a given color
            auto breakdown_color = [&](COLOUR parent_color)
            {
                pair<COLOUR, COLOUR> source_colors;
                switch (parent_color)
                {
                case FG_BLUE:
                    source_colors = { FG_BLUE, FG_BLUE };
                    break;
                case FG_RED:
                    source_colors = { FG_RED, FG_RED };
                    break;
                case FG_GREEN:
                    source_colors = { FG_GREEN, FG_GREEN };
                    break;
                case FG_MAGENTA:
                    source_colors = { FG_BLUE, FG_RED };
                    break;
                case FG_CYAN:
                    source_colors = { FG_BLUE, FG_GREEN };
                    break;
                case FG_YELLOW:
                    source_colors = { FG_RED, FG_GREEN };
                    break;
                }

                return source_colors;
            };

            // Labda to return a pair of color that represent inherited color
            auto punnit_color = [&](COLOUR parentA, COLOUR parentB)
            {
                auto parentBreakdownA = breakdown_color(parentA);
                auto parentBreakdownB = breakdown_color(parentB);

                vector<pair<COLOUR, COLOUR>>chances;

                for (int x = 0; x < 2; x++)
                    for (int y = 0; y < 2; y++)
                    {
                        pair<COLOUR, COLOUR> newPair = { x == 0 ? parentBreakdownA.first : parentBreakdownA.second,
                                                         y == 0 ? parentBreakdownB.first : parentBreakdownB.second };
                        newPair.first = rand() % 100 <= 1 ? m_colors[rand() % 3] : newPair.first;
                        newPair.second = rand() % 100 <= 1 ? m_colors[rand() % 3] : newPair.second;

                        chances.push_back(newPair);
                    }

                return chances[rand() % 4];
            };

            // Tab to change fittest color
            if (GetKey(VK_TAB).bPressed)
            {
                selectedFitColor = (FitColor)((selectedFitColor + 1) % NUM_FIT_COLOR);
                switch (selectedFitColor)
                {
                case FIT_RED:
                    _fittest_color = FG_RED;
                    break;
                case FIT_BLUE:
                    _fittest_color = FG_BLUE;
                    break;
                case FIT_GREEN:
                    _fittest_color = FG_GREEN;
                    break;
                case FIT_CYAN:
                    _fittest_color = FG_CYAN;
                    break;
                case FIT_MAGENTA:
                    _fittest_color = FG_MAGENTA;
                    break;
                case FIT_YELLOW:
                    _fittest_color = FG_YELLOW;
                    break;
                case FIT_RANDOM:
                    _fittest_color = FG_GREEN;
                    break;
                }
            }

            // store current state
            for (int i = 0; i < ScreenWidth() * gameHeight; i++)
            {
                m_output[i] = m_state[i];
            }

            // Iterate every cell
            for (int x = 0; x < ScreenWidth(); x++)
                for (int y = 0; y < gameHeight; y++)
                {
                    // Find neighbors
                    int nNeighbours = cell(x - 1, y - 1) + cell(x, y - 1) + cell(x + 1, y - 1) +
                                      cell(x - 1, y    )                  + cell(x + 1, y    ) +
                                      cell(x - 1, y + 1) + cell(x, y + 1) + cell(x + 1, y + 1);

                    // If cell is currently alive
                    if (cell(x, y) == 1)
                    {
                        // Set alive based on neighbors
                        m_state[y * ScreenWidth() + x] = nNeighbours == 2 || nNeighbours == 3;
                        m_color[y * ScreenWidth() + x] = cell_color(x, y);
                    }
                    else
                    {
                        // Offsrping time
                        if (nNeighbours == 3)
                        {
                            // Find potential parents
                            vector<pair<int, int>> parents;
                            for (int par_x = -1; par_x < 2; par_x++)
                                for (int par_y = -1; par_y < 2; par_y++)
                                    if (cell(x + par_x, y + par_y))
                                    {
                                        pair<int, int> parent = { x + par_x, y + par_y };
                                        parents.push_back(parent);
                                    }

                            // Set chances for parents
                            vector<int> chances = { 0, 1, 2 };
                            int totalchances = 3;
                            for(int i = 0; i < 3; i++)
                                if (cell_color(parents[i].first, parents[i].second) == _fittest_color)
                                {
                                    totalchances++;
                                    chances[i]++;

                                    if (i < 2)
                                        for (int j = i + 1; j < 3; j++)
                                            chances[j]++;
                                }

                            vector<int> parent_sel;

                            // Chose 2 parents
                            while (parent_sel.size() <= 2)
                            {
                                int rand_sel = rand() % totalchances;

                                // Remove from chances when selected
                                for(int i = 0; i < chances.size(); i++)
                                    if (rand_sel <= chances[i])
                                    {
                                        parent_sel.push_back(i);
                                        chances[i] += totalchances + 1;
                                    }
                            }

                            // Inherit color
                            pair<COLOUR, COLOUR> parent_colors = punnit_color( cell_color(parents[parent_sel[0]].first, parents[parent_sel[0]].second),
                                cell_color(parents[parent_sel[1]].first, parents[parent_sel[1]].second));
                            m_color[y * ScreenWidth() + x] = color_mixing_map[parent_colors];
                            m_state[y * ScreenWidth() + x] = 1;
                        }
                        else
                        {
                            // No life
                            m_color[y * ScreenWidth() + x] = FG_BLACK;
                            m_state[y * ScreenWidth() + x] = 0;
                        }

                    }

                    // If alive
                    if (cell(x, y) == 1)
                    {
                        // Draw life
                        Draw(x, y, PIXEL_SOLID, m_color[x + y * ScreenWidth()]);
                        
                        // Count life
                        numLife++;
                    }
                    else
                        // No life, black pixel
                        Draw(x, y, PIXEL_SOLID, FG_BLACK);
                }

            // Display speed
            wstring speed = L"Speed: " + std::to_wstring((int)sleepTime.count()) + L"ms between frames";
            DrawString(2, gameHeight + 1, speed);
            wstring numLifeStr = L"Num Life: " + std::to_wstring(numLife);
            DrawString(2, gameHeight + 2, numLifeStr);

            // Display fittest color
            wstring seeding = L"FittestColor(TAB to change): ";
            DrawString(2, gameHeight + 3, seeding);
            wstring seeding_color = L"";

            switch (selectedFitColor)
            {
            case FIT_RED:
                DrawString(29, gameHeight + 3, L"RED", FG_RED);
                break;
            case FIT_GREEN:
                DrawString(29, gameHeight + 3, L"GREEN", FG_GREEN);
                break;
            case FIT_BLUE:
                DrawString(29, gameHeight + 3, L"BLUE", FG_BLUE);
                break;
            case FIT_CYAN:
                DrawString(29, gameHeight + 3, L"CYAN", FG_CYAN);
                break;
            case FIT_MAGENTA:
                DrawString(29, gameHeight + 3, L"MAGENTA", FG_MAGENTA);
                break;
            case FIT_YELLOW:
                DrawString(29, gameHeight + 3, L"YELLOW", FG_YELLOW);
                break;
            case FIT_RANDOM:
                wstring rand_str = L"RANDOM";

                // Animate random letters
                static int colorAnim = 0;
                for (int i = 0; i < rand_str.size(); i++)
                {
                    wstring rand_char_to_str = L"";
                    rand_char_to_str.push_back(rand_str[i]);
                    DrawString(29 + i, gameHeight + 3, rand_char_to_str, m_fit_colors[(colorAnim / 10 + i) % 6]);
                }
                colorAnim++;

                // Update random fittest color every 150 frames
                _fittest_color_timer++;
                if ((_fittest_color_timer = _fittest_color_timer % 150) == 0)
                {
                    _fittest_color = m_fit_colors[rand() % 6];
                }
                switch (_fittest_color)
                {
                case FG_RED:
                    DrawString(36, gameHeight + 3, L"(RED)", FG_RED);
                    break;
                case FG_GREEN:
                    DrawString(36, gameHeight + 3, L"(GREEN)", FG_GREEN);
                    break;
                case FG_BLUE:
                    DrawString(36, gameHeight + 3, L"(BLUE)", FG_BLUE);
                    break;
                case FG_CYAN:
                    DrawString(36, gameHeight + 3, L"(CYAN)", FG_CYAN);
                    break;
                case FG_MAGENTA:
                    DrawString(36, gameHeight + 3, L"(MAGENTA)", FG_MAGENTA);
                    break;
                case FG_YELLOW:
                    DrawString(36, gameHeight + 3, L"(YELLOW)", FG_YELLOW);
                    break;
                }
                break;
            }
        }
        
        // Back to main menu
        if (GetKey(VK_F5).bPressed)
            currentState = INSTRUCTION;

        // Speed up
        if (GetKey(VK_LSHIFT).bPressed)
            sleepTime = (sleepTime - 5ms) > 0ms ? (sleepTime - 5ms) : 0ms;

        // Slow down
        if (GetKey(VK_LCONTROL).bPressed)
            sleepTime = sleepTime + 5ms;
        
        // Quit
        if (GetKey(VK_ESCAPE).bPressed)
          return false;

        return true;
    }

    bool OnUserDestroy()override 
    { 
        return true; 
    }
};

int main()
{
    // Make game of life
    gameOfLife game;
    game.ConstructConsole(100, 60, 10, 10);
    game.Start();
}
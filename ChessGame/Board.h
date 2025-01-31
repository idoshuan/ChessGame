#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Square.h"

class Board {
private:

    static constexpr size_t len = 8;
    std::vector<std::vector<Square>> board;

public:

    Board();
    void draw(sf::RenderWindow& window) const;
    
};
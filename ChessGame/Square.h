#pragma once

#include <SFML/Graphics.hpp>

class Square {
private:
    static constexpr float square_size = 135;

    sf::RectangleShape square;
    sf::Vector2f pos;

public:
    Square(size_t row, size_t col, bool isBlack) : pos(sf::Vector2f(col* square_size, row* square_size)) {
        square.setSize(sf::Vector2f(square_size, square_size));
        square.setPosition(pos);
        square.setFillColor(isBlack ? sf::Color(50, 50, 50) : sf::Color(200, 200, 200));
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(square);
    }
};
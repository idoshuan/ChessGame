#include <SFML/Graphics.hpp>
#include <iostream>

#include "Board.h"

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "SFML works!");

    Board board;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        window.clear();
        board.draw(window);
        window.display();
    }
}
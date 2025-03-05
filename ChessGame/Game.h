#pragma once

#include <algorithm>

#include "ChessBoard.h"
#include "Piece.h"

class Game {
public:
    Game();
    void run();

private:
    ChessBoard chessBoard;
    sf::RenderWindow window;

    int fullMoveCount;
    int halfMoveClock;
    bool isWhiteTurn;
    std::string enPassantTarget;

    Piece* selectedPiece;
    sf::Vector2f dragOffset;
    sf::Vector2f origianlPosition;

    void handleEvents(const std::optional<sf::Event>& event);
    void onPieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed);
    void onPieceReleased(const sf::Event::MouseButtonReleased* mouseButtonReleased);
};
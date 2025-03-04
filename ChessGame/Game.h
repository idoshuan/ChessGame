#pragma once
#include "ChessBoard.h"

class Game {
public:
    Game();
    void run();
private:
    ChessBoard chessBoard;
    sf::RenderWindow window;

    int fullMoveNumber;
    int halfMoveClock;
    bool whiteToPlay;
    bool wk, wq, bk, bq;
    std::string enPassantTarget;

    bool isDragging;
    char selectedPiece;
    int selectedRow, selectedCol;
    sf::Vector2f dragOffset;
    std::optional<sf::Sprite> dragSprite;

    void handleEvents(std::optional<sf::Event>& event);
    void pieceClicked(const sf::Event::MouseButtonPressed * mouseButtonPressed);
    void pieceReleased(const sf::Event::MouseButtonReleased* mouseButtonPressed);
    std::string castlingRightsToStr();
};

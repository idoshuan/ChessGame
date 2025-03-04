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
    bool whiteKingCastle, whiteQueenCastle, blackKingCastle, blackQueenCastle;
    std::string enPassantTarget;

    bool isDragging;
    Piece* selectedPiece;
    std::vector<Square> legalMoves;
    std::optional<sf::Sprite> draggedSprite;
    sf::Vector2f dragOffset;

    void handleEvents(const std::optional<sf::Event>& event);
    void onPieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed);
    void onPieceReleased(const sf::Event::MouseButtonReleased* mouseButtonReleased);
    std::string getCastlingRights() const;
};
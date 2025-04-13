#pragma once

#include <algorithm>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream> 
#include <future>

#include "ChessBoard.h"
#include "Piece.h"
#include "Stockfish.h"

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

    void handleEvents(const std::optional<sf::Event>& event, bool& isRunning);
    void onPieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed);
    void onPieceReleased(const sf::Event::MouseButtonReleased* mouseButtonReleased);
    void applyStockfishMove(const std::string& move);

    void runStockfish(const std::string& fen, int n);

    Stockfish stockfish;
    std::future<std::vector<std::string>> stockfishFuture;
    bool isAwaitingStockfish = false;
};


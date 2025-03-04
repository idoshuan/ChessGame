#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <cstdlib>
#include <array>

class ChessBoard {
public:
    ChessBoard();

    bool loadTextures();
    void draw(sf::RenderWindow& window, int selectedRow, int selectedCol);
    void movePiece(int fromRow, int fromCol, int toRow, int toCol);
    std::vector<std::string> getLegalMovesFromStockfish(const std::string& fen);
    std::string generateFEN(std::string castlingRights, bool whiteToPlay, std::string enPassant, int halfmoveClock, int fullmoveNumber);
    std::string boardToFEN();
    void updateCastleRights(char piece, int fromRow, int fromCol, bool& wk, bool& wq, bool& bk, bool& bq);
    std::string getEnPassantTarget(int fromRow, int toRow, int col, char piece);
    const sf::Texture& getPieceTexture(char piece) {
        return pieceTextures[piece];
    }
    char getPiece(int row, int col) const {
        return currBoard[row][col];
    }
    std::string indexToLiteral(int row, int col) {
        return std::string(1, 'a' + col) + std::to_string(8 - row);
    }
    static float getSquareSize() { 
        return squareSize; 
    }
private:
    static constexpr int boardSize = 8;
    static constexpr int windowSize = 1200;
    static const float squareSize;
    static const char originalBoard[boardSize][boardSize];

    char currBoard[8][8];

    std::map<char, sf::Texture> pieceTextures;
    sf::RenderTexture boardTexture;
    sf::Sprite boardSprite;
};



#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <iostream>
#include <vector>
#include <string>

#include "Constants.h"
#include "Piece.h"
#include "Pawn.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "King.h"

class ChessBoard {
public:
    ChessBoard();

    Piece* generatePiece(int row, int col);
    bool loadTextures();
    void draw(sf::RenderWindow& window, Piece* selectedPiece) const;
    void movePiece(int fromRow, int fromCol, int toRow, int toCol);
    std::string generateFEN(const std::string& castlingRights, bool isWhiteTurn, const std::string& enPassant, int halfMoveClock, int fullMoveCount) const;
    std::string boardToFEN() const;
    void updateCastleRights(Piece* piece, bool& whiteKingCastle, bool& whiteQueenCastle, bool& blackKingCastle, bool& blackQueenCastle);
    std::string getEnPassantTarget(Piece* piece, int toRow) const;

    const sf::Texture& getPieceTexture(Piece* piece) const {
        return pieceTextures.at(piece->getType());
    }

    Piece* getPiece(int row, int col) const {
        return board[row][col];
    }

    std::string indexToLiteral(int row, int col) const {
        return std::string(1, 'a' + col) + std::to_string(8 - row);
    }

    bool isSquareOccupied(const Square& pos) const {
        return board[pos.row][pos.col] != nullptr;
    }

private:
    static constexpr char initialBoard[BOARD_SIZE][BOARD_SIZE] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    Piece* board[BOARD_SIZE][BOARD_SIZE];
    std::map<PieceType, sf::Texture> pieceTextures;
    sf::RenderTexture boardTexture;
    sf::Sprite boardSprite;
};

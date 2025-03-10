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

    bool loadTextures();
    void draw(sf::RenderWindow& window, Piece* selectedPiece) const;

    Piece* generatePiece(int row, int col);
    void movePiece(Square fromSquare, Square toSquare);
    std::vector<Square> getLegalMoves(Piece* piece);

    void updateCastleRights(Piece* piece);
    void updateEnPassant(Piece* movedPiece, Square from, Square to);
    std::string getCastlingRights() const;

    bool isCheckMate(bool isWhite);
    bool doesMoveLeaveKingInCheck(Square from, Square to);
    bool isKingInCheck(bool isWhite) const;

    std::string generateFEN(bool isWhiteTurn, int halfMoveClock, int fullMoveCount) const;
    std::string boardToFEN() const;

    const sf::Texture& getPieceTexture(Piece* piece) const {
        return pieceTextures.at(piece->getType());
    }

    Piece* getPiece(Square square) const {
        return board[square.row][square.col];
    }

    bool isSquareValid(Square square) const {
        return square.row < BOARD_SIZE && square.col < BOARD_SIZE && square.row >= 0 && square.col >= 0;
    }

    bool isSquareOccupied(const Square& square) const {
        return board[square.row][square.col] != nullptr;
    }

    static Square literalToSquare(std::string s) {
        return { 8 - (s[1] - '0'), s[0] - 'a' };
    }

    static std::string squareToLiteral(Square square) {
        return std::string(1, 'a' + square.col) + std::to_string(8 - square.row);
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

    Piece* board[BOARD_SIZE][BOARD_SIZE]{};
    sf::RenderTexture boardTexture;
    sf::Sprite boardSprite;
    std::map<PieceType, sf::Texture> pieceTextures;
    sf::Font font;

    Piece *whiteKing, *blackKing;
    Square enPassantTarget;
    Square castleTarget;
    bool whiteKingCastle, whiteQueenCastle, blackKingCastle, blackQueenCastle;
};

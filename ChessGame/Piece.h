#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Constants.h"

class ChessBoard;

enum class PieceType {
    W_KING = 'K',
    W_QUEEN = 'Q',
    W_ROOK = 'R',
    W_BISHOP = 'B',
    W_KNIGHT = 'N',
    W_PAWN = 'P',
    B_KING = 'k',
    B_QUEEN = 'q',
    B_ROOK = 'r',
    B_BISHOP = 'b',
    B_KNIGHT = 'n',
    B_PAWN = 'p',
    NONE = '.'
};

enum class Color {
    WHITE, 
    BLACK
};

struct Square {
    int row;
    int col;

    bool operator==(const Square& other) const {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Square& other) const {
        return row != other.row || col != other.col;
    }
};

class Piece : public sf::Sprite {
public:
    Piece(Color color, const sf::Texture& texture, PieceType type, Square pos, ChessBoard* board) : sf::Sprite(texture), type(type), pos(pos), board(board) {
        setScale(sf::Vector2f(SQUARE_SIZE / getTexture().getSize().x, SQUARE_SIZE / getTexture().getSize().y));
        setPosition(sf::Vector2f(pos.col * SQUARE_SIZE, pos.row * SQUARE_SIZE));
    }
    virtual ~Piece() = default;
    virtual std::vector<Square> getLegalMoves() const = 0;
    static PieceType charToPieceType(char ch);
    static char pieceTypeToChar(PieceType type);
    void setSquare(Square newPos) {
        pos = newPos;
    }
    Square getSquare() const {
        return pos;
    }
    PieceType getType() const {
        return type;
    }
protected:
    PieceType type;
    Square pos;
    ChessBoard* board;
};
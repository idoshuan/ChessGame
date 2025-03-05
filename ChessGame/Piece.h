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
	Square operator+(const Square& other) const {
		return { row + other.row, col + other.col };
	}
	Square& operator+=(const Square& other) {
		*this = *this + other; 
		return *this;
	}
};

class Piece : public sf::Sprite {
public:
	Piece(Color color, const sf::Texture& texture, PieceType type, Square square, ChessBoard* board) : sf::Sprite(texture), type(type), color(color), square(square), board(board) {
		setScale(sf::Vector2f(SQUARE_SIZE / getTexture().getSize().x, SQUARE_SIZE / getTexture().getSize().y));
		setPosition(sf::Vector2f(square.col * SQUARE_SIZE, square.row * SQUARE_SIZE));
	}
	virtual ~Piece() = default;
	virtual std::vector<Square> getPossibleMoves(const std::string& enPassantTarget) const = 0;
	static Piece* createPiece(PieceType type, Color color, const sf::Texture& texture, Square position, ChessBoard* board);
	static PieceType charToPieceType(char ch);
	static char pieceTypeToChar(PieceType type);
	void setSquare(Square newSquare) {
		square = newSquare;
	}
	const Square getSquare() const {
		return square;
	}
	PieceType getType() const {
		return type;
	}
	bool isWhite() const {
		return color == Color::WHITE;
	}
protected:
	PieceType type;
	Color color;
	Square square;
	ChessBoard* board;
};
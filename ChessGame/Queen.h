#pragma once

#include "Piece.h"

class Queen : public Piece {
public:
	Queen(Color color, const sf::Texture& texture, Square pos, ChessBoard* board) : Piece(color, texture, (color == Color::WHITE ? PieceType::W_QUEEN : PieceType::B_QUEEN), pos, board) {}
	std::vector<Square> getLegalMoves() const override;
};
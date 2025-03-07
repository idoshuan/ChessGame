#pragma once

#include "Piece.h"

class King : public Piece {
public:
	King(Color color, const sf::Texture& texture, Square pos, ChessBoard* board) : Piece(color, texture, (color == Color::WHITE ? PieceType::W_KING : PieceType::B_KING), pos, board) {}
	std::vector<Square> getPossibleMoves() const override;
};
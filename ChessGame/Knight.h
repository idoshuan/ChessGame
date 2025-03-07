#pragma once

#include "Piece.h"

class Knight : public Piece {
public:
	Knight(Color color, const sf::Texture& texture, Square pos, ChessBoard* board) : Piece(color, texture, (color == Color::WHITE ? PieceType::W_KNIGHT : PieceType::B_KNIGHT), pos, board) {}
	std::vector<Square> getPossibleMoves() const override;
};
#pragma once

#include "Piece.h"

class Rook : public Piece {
public:
	Rook(Color color, const sf::Texture& texture, Square pos, ChessBoard* board) : Piece(color, texture, (color == Color::WHITE ? PieceType::W_ROOK: PieceType::B_ROOK), pos, board) {}
	std::vector<Square> getPossibleMoves(const Square enPassantTarget) const override;
};
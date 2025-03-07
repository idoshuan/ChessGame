#pragma once

#include "Piece.h"

class Bishop : public Piece {
public:
	Bishop(Color color, const sf::Texture& texture, Square pos, ChessBoard* board) : Piece(color, texture, (color == Color::WHITE ? PieceType::W_BISHOP : PieceType::B_BISHOP), pos, board) {}
	std::vector<Square> getPossibleMoves() const override;
};
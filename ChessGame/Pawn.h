#pragma once

#include "Piece.h"

class Pawn : public Piece {
public:
	Pawn(Color color, const sf::Texture& texture, Square pos, ChessBoard* board) : Piece(color, texture, (color == Color::WHITE ? PieceType::W_PAWN : PieceType::B_PAWN), pos, board){}
	std::vector<Square> getPossibleMoves(const std::string& enPassantTarget) const override;
};
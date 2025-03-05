#include "Knight.h"
#include "ChessBoard.h"

std::vector<Square> Knight::getPossibleMoves(const std::string& enPassantTarget) const {
	std::vector<Square> moves;
	Square directions[] = { {2, 1}, {2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {-2, 1}, {-2, -1} }; // L-shape moves

	for (Square dir : directions) {
		Square currSquare = square + dir;

		if (!board->isSquareValid(currSquare)) continue; // Stop if out of bounds

		if (board->isSquareOccupied(currSquare)) {
			// If occupied by teammate, continue
			Piece* target = board->getPiece(currSquare);
			if (!target || target->isWhite() == this->isWhite()) {
				continue;
			}
		}

		moves.push_back(currSquare);
	}

	return moves;
}

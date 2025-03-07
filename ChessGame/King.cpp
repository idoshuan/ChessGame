#include "King.h"
#include "ChessBoard.h"

std::vector<Square> King::getPossibleMoves() const {
	std::vector<Square> moves;
	Square directions[] = { {1, 1}, {-1, -1}, {1, -1}, {-1, 1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1} }; // All Directions

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

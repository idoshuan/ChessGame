#include "Pawn.h"
#include "ChessBoard.h"

std::vector<Square> Pawn::getPossibleMoves() const {
	std::vector<Square> moves;
	int dir = (type == PieceType::W_PAWN) ? -1 : 1; // White moves up (-1), Black moves down (+1)
	int startRow = (type == PieceType::W_PAWN) ? 6 : 1;

	// Movement (1 or 2 squares forward)
	for (int step = 1; step <= (square.row == startRow ? 2 : 1); ++step) {
		Square nextSquare = { square.row + step * dir, square.col };
		if (!board->isSquareValid(nextSquare) || board->isSquareOccupied(nextSquare)) break;
		moves.push_back(nextSquare);
	}

	// Captures (left & right diagonals)
	Square captureOffsets[] = { {dir, -1}, {dir, 1} };
	for (Square offset : captureOffsets) {
		Square captureSquare = { square.row + offset.row, square.col + offset.col };
		if (board->isSquareValid(captureSquare) && board->isSquareOccupied(captureSquare)) {
			Piece* target = board->getPiece(captureSquare);
			if (target && target->isWhite() != this->isWhite()) {
				moves.push_back(captureSquare);
			}
		}
	}

	return moves;
}



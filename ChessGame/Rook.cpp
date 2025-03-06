#include "Rook.h"
#include "ChessBoard.h"

std::vector<Square> Rook::getPossibleMoves(const Square enPassantTarget) const {
    std::vector<Square> moves;
    Square directions[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} }; // Right, Left, Down, Up

    for (Square dir : directions) {
        Square currSquare = square;
        while (true) {
            currSquare += dir;

            if (!board->isSquareValid(currSquare)) break; // Stop if out of bounds

            if (board->isSquareOccupied(currSquare)) {
                // If occupied by opponent, add move before stopping
                Piece* target = board->getPiece(currSquare);
                if (target && target->isWhite() != this->isWhite()) {
                    moves.push_back(currSquare);
                }
                break;
            }

            moves.push_back(currSquare);
        }
    }

    return moves;
}
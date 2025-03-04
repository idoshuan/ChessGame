#include "Pawn.h"
#include "ChessBoard.h"

std::vector<Square> Pawn::getLegalMoves(const std::string& enPassantTarget) const {
    std::vector<Square> moves;

    int dir = (type == PieceType::W_PAWN) ? -1 : 1; // White moves up (-1), Black moves down (+1)
    int startRow = (type == PieceType::W_PAWN) ? 6 : 1; 

    // Regular movement
    Square firstSquare = { square.row + dir, square.col };
    Square secondSquare = { square.row + 2 * dir, square.col };

    bool firstOccupied = board->isSquareOccupied(firstSquare);
    bool secondOccupied = board->isSquareOccupied(secondSquare);

    if (!firstOccupied) {
        moves.push_back(firstSquare);

        if (square.row == startRow && !secondOccupied) {
            moves.push_back(secondSquare);
        }
    }

    // Regular captures
    Square leftCapture = { square.row + dir, square.col - 1 };
    Square rightCapture = { square.row + dir, square.col + 1 };

    if (square.col > 0 && board->isSquareOccupied(leftCapture)) { // Left diagonal capture
        Piece* target = board->getPiece(leftCapture.row, leftCapture.col);
        if (target && target->isWhite() != this->isWhite()) {
            moves.push_back(leftCapture);
        }
    }

    if (square.col < BOARD_SIZE - 1 && board->isSquareOccupied(rightCapture)) { // Right diagonal capture
        Piece* target = board->getPiece(rightCapture.row, rightCapture.col);
        if (target && target->isWhite() != this->isWhite()) {
            moves.push_back(rightCapture);
        }
    }

    // En Passant Capture (if applicable)
    if (enPassantTarget != "-") {
        int enPassantCol = enPassantTarget[0] - 'a';
        int enPassantRow = 8 - (enPassantTarget[1] - '0');
        // Check if en passant square is valid for this pawn
        if (enPassantRow == square.row + dir && (enPassantCol == square.col - 1 || enPassantCol == square.col + 1)) {
            moves.push_back({ enPassantRow, enPassantCol });
        }
    }

    return moves;
}


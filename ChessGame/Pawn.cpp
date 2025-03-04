#include "Pawn.h"
#include "ChessBoard.h"

std::vector<Square> Pawn::getLegalMoves() const {
    std::vector<Square> moves;

    int dir = (type == PieceType::W_PAWN) ? -1 : 1; // White moves up (-1), Black moves down (+1)
    int startRow = (type == PieceType::W_PAWN) ? 6 : 1; 

    Square firstSquare = { pos.row + dir, pos.col };
    Square secondSquare = { pos.row + 2 * dir, pos.col };
    bool firstOccupied = board->isSquareOccupied(firstSquare);
    bool secondOccupied = board->isSquareOccupied(secondSquare);


    if (!firstOccupied) {
        moves.push_back(firstSquare);

        if (pos.row == startRow && !secondOccupied) {
            moves.push_back(secondSquare);
        }
    }

    return moves;
}


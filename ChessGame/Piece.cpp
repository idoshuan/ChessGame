#include "Piece.h"

PieceType Piece::charToPieceType(char ch) {
    switch (ch) {
    case 'K': return PieceType::W_KING;
    case 'Q': return PieceType::W_QUEEN;
    case 'R': return PieceType::W_ROOK;
    case 'B': return PieceType::W_BISHOP;
    case 'N': return PieceType::W_KNIGHT;
    case 'P': return PieceType::W_PAWN;
    case 'k': return PieceType::B_KING;
    case 'q': return PieceType::B_QUEEN;
    case 'r': return PieceType::B_ROOK;
    case 'b': return PieceType::B_BISHOP;
    case 'n': return PieceType::B_KNIGHT;
    case 'p': return PieceType::B_PAWN;
    default:  return PieceType::NONE;
    }
}

char Piece::pieceTypeToChar(PieceType type) {
    switch (type) {
    case PieceType::W_PAWN:   return 'P';
    case PieceType::B_PAWN:   return 'p';
    case PieceType::W_ROOK:   return 'R';
    case PieceType::B_ROOK:   return 'r';
    case PieceType::W_KNIGHT: return 'N';
    case PieceType::B_KNIGHT: return 'n';
    case PieceType::W_BISHOP: return 'B';
    case PieceType::B_BISHOP: return 'b';
    case PieceType::W_QUEEN:  return 'Q';
    case PieceType::B_QUEEN:  return 'q';
    case PieceType::W_KING:   return 'K';
    case PieceType::B_KING:   return 'k';
    case PieceType::NONE:     return '.'; // Represent an empty square
    default:                   return '?'; // Unknown piece type
    }
}


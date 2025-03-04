#include "ChessBoard.h"

ChessBoard::ChessBoard() : boardTexture(sf::Vector2u(WINDOW_SIZE, WINDOW_SIZE)), boardSprite(boardTexture.getTexture()) {

	if (!loadTextures()) {
		std::cerr << "Error loading textures!" << std::endl;
	}

	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			sf::RectangleShape square({ SQUARE_SIZE, SQUARE_SIZE });
			square.setPosition({ col * SQUARE_SIZE, row * SQUARE_SIZE });
			square.setFillColor((row + col) % 2 == 0 ? sf::Color::White : sf::Color(118, 150, 86));
			boardTexture.draw(square);
			board[row][col] = generatePiece(row, col);
		}
	}
	boardTexture.display();
	boardSprite.setTexture(boardTexture.getTexture());
}


Piece* ChessBoard::generatePiece(int row, int col) {
	auto type = Piece::charToPieceType(initialBoard[row][col]);
	if (type == PieceType::NONE) return nullptr;

	Color color = std::isupper(initialBoard[row][col]) ? Color::WHITE : Color::BLACK;
	Square position = { row, col };
	const sf::Texture& texture = pieceTextures.at(type);

	switch (type) {
	case PieceType::W_PAWN: case PieceType::B_PAWN:
		return new Pawn(color, texture, position, this);
	case PieceType::W_KNIGHT: case PieceType::B_KNIGHT:
		return new Knight(color, texture, position, this);
	case PieceType::W_BISHOP: case PieceType::B_BISHOP:
		return new Bishop(color, texture, position, this);
	case PieceType::W_ROOK: case PieceType::B_ROOK:
		return new Rook(color, texture, position, this);
	case PieceType::W_QUEEN: case PieceType::B_QUEEN:
		return new Queen(color, texture, position, this);
	case PieceType::W_KING: case PieceType::B_KING:
		return new King(color, texture, position, this);
	default:
		return nullptr;
	}
}

bool ChessBoard::loadTextures() {
	std::map<PieceType, std::string> textureFiles = {
		{PieceType::W_PAWN,   "pieces-png/wp.png"},
		{PieceType::B_PAWN,   "pieces-png/bp.png"},
		{PieceType::W_ROOK,   "pieces-png/wr.png"},
		{PieceType::B_ROOK,   "pieces-png/br.png"},
		{PieceType::W_KNIGHT, "pieces-png/wn.png"},
		{PieceType::B_KNIGHT, "pieces-png/bn.png"},
		{PieceType::W_BISHOP, "pieces-png/wb.png"},
		{PieceType::B_BISHOP, "pieces-png/bb.png"},
		{PieceType::W_QUEEN,  "pieces-png/wq.png"},
		{PieceType::B_QUEEN,  "pieces-png/bq.png"},
		{PieceType::W_KING,   "pieces-png/wk.png"},
		{PieceType::B_KING,   "pieces-png/bk.png"}
	};

	for (const auto& [piece, file] : textureFiles) {
		if (!pieceTextures[piece].loadFromFile(file)) {
			std::cerr << "Failed to load texture: " << file << std::endl;
			return false;
		}
		pieceTextures[piece].setSmooth(true);
	}
	return true;
}


void ChessBoard::draw(sf::RenderWindow& window, Piece* selectedPiece) const {
	window.draw(boardSprite);
	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			Piece* piece = board[row][col];
			if (piece && piece != selectedPiece) {
				window.draw(*piece);
			}
		}
	}
}


void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
	board[toRow][toCol] = board[fromRow][fromCol];
	board[fromRow][fromCol] = nullptr;
	board[toRow][toCol]->setSquare({ toRow, toCol });
	board[toRow][toCol]->setPosition({ toCol * SQUARE_SIZE, toRow * SQUARE_SIZE });
	//std::cout << indexToLiteral(fromRow, fromCol) << indexToLiteral(toRow, toCol) << "\n";
}


std::string ChessBoard::generateFEN(const std::string& castlingRights, bool isWhiteTurn, const std::string& enPassant, int halfMoveClock, int fullMoveCount) const {
	return boardToFEN() + " " + (isWhiteTurn ? "w" : "b") + " " + (castlingRights.empty() ? "-" : castlingRights) + " " + (enPassant.empty() ? "-" : enPassant) + " " + std::to_string(halfMoveClock) + " " + std::to_string(fullMoveCount);
}

std::string ChessBoard::boardToFEN() const {
	std::string fen;
	for (int row = 0; row < BOARD_SIZE; ++row) {
		int emptyCount = 0;
		for (int col = 0; col < BOARD_SIZE; ++col) {
			char piece = Piece::pieceTypeToChar(board[row][col] ? board[row][col]->getType() : PieceType::NONE);
			if (piece == '.') {
				emptyCount++;
			}
			else {
				if (emptyCount > 0) {
					fen += std::to_string(emptyCount);
					emptyCount = 0;
				}
				fen += piece;
			}
		}
		if (emptyCount > 0) fen += std::to_string(emptyCount);
		if (row < BOARD_SIZE - 1) fen += "/";
	}
	return fen;
}

void ChessBoard::updateCastleRights(Piece* piece, bool& wk, bool& wq, bool& bk, bool& bq) {
	int fromRow = piece->getSquare().row;
	int fromCol = piece->getSquare().col;

	switch (piece->getType()) {
	case PieceType::W_KING:
		wk = false; wq = false;
		break;
	case PieceType::B_KING:
		bk = false; bq = false;
		break;
	case PieceType::W_ROOK:
		if (fromRow == 7) {
			if (fromCol == 0) wq = false;
			else if (fromCol == 7) wk = false;
		}
		break;
	case PieceType::B_ROOK:
		if (fromRow == 0) {
			if (fromCol == 0) bq = false;
			else if (fromCol == 7) bk = false;
		}
		break;
	default:
		break;
	}
}

std::string ChessBoard::getEnPassantTarget(Piece* piece, int toRow) const {
	if (piece->getType() == PieceType::W_PAWN && piece->getSquare().row == 6 && toRow == 4) { // White pawn double move
		return std::string(1, 'a' + piece->getSquare().col) + "3";
	}
	else if (piece->getType() == PieceType::B_PAWN && piece->getSquare().row == 1 && toRow == 3) { // Black pawn double move
		return std::string(1, 'a' + piece->getSquare().col) + "6";
	}
	return "-"; // No en passant
}
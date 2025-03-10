#include "ChessBoard.h"

/**
 * @brief  Initializes the chessboard and loads textures.
 *
 * The board background is drawn once into a texture for performance optimization.
 * It also initializes castling rights and sets up the pieces.
 */
ChessBoard::ChessBoard() : boardTexture(sf::Vector2u(WINDOW_SIZE, WINDOW_SIZE)), boardSprite(boardTexture.getTexture()), whiteKing(nullptr), blackKing(nullptr), enPassantTarget({ -1,-1 }), castleTarget({ -1,-1 }), whiteKingCastle(true), whiteQueenCastle(true), blackKingCastle(true), blackQueenCastle(true) {

	if (!font.openFromFile("fonts/arial.ttf")) {
		std::cerr << "Failed to load font!" << std::endl;
		return;
	}

	if (!loadTextures()) {
		std::cerr << "Error loading textures!" << std::endl;
	}

	sf::Text label(font);

	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			// Draw the chessboard squares
			sf::RectangleShape square({ SQUARE_SIZE, SQUARE_SIZE });
			square.setPosition({ col * SQUARE_SIZE, row * SQUARE_SIZE });
			square.setFillColor((row + col) % 2 == 0 ? sf::Color::White : sf::Color(118, 150, 86));
			boardTexture.draw(square);

			board[row][col] = generatePiece(row, col);
			if (board[row][col] && board[row][col]->getType() == PieceType::W_KING) {
				whiteKing = board[row][col];
			}
			else if (board[row][col] && board[row][col]->getType() == PieceType::B_KING) {
				blackKing = board[row][col];
			}

			// Determine text color to contrast the square color
			sf::Color textColor = (row + col) % 2 == 0 ? sf::Color(118, 150, 86) : sf::Color::White;

			// Draw column labels (a-h) at the bottom
			if (row == BOARD_SIZE - 1) {
				label.setString(static_cast<char>('a' + col));
				label.setPosition({ (col + 1) * SQUARE_SIZE - 22 ,  (row + 1) * SQUARE_SIZE - 35 });
				label.setFillColor(textColor);
				boardTexture.draw(label);
			}

			// Draw row labels (1-8) on the left side
			if (col == 0) {
				label.setString(std::to_string(8 - row));
				label.setPosition({ col * SQUARE_SIZE + 5, row * SQUARE_SIZE + 5 });
				label.setFillColor(textColor);
				boardTexture.draw(label);
			}
		}
	}
	boardTexture.display();
	boardSprite.setTexture(boardTexture.getTexture());
}

/**
 * @brief  Creates a chess piece based on the initial board setup.
 *
 * Uses the predefined `initialBoard` array to generate each piece at its
 * starting position.
 */
Piece* ChessBoard::generatePiece(int row, int col) {
	auto type = Piece::charToPieceType(initialBoard[row][col]);
	if (type == PieceType::NONE) return nullptr;

	Color color = std::isupper(initialBoard[row][col]) ? Color::WHITE : Color::BLACK;
	Square position = { row, col };
	const sf::Texture& texture = pieceTextures.at(type);
	return Piece::createPiece(type, color, pieceTextures.at(type), position, this);
}

/**
 * @brief  Loads all piece textures once to improve performance.
 *
 * Reduces disk I/O by caching textures in memory, ensuring smooth rendering.
 */
bool ChessBoard::loadTextures() {
	std::map<PieceType, std::string> textureFiles = {
		{PieceType::W_PAWN, "pieces-png/wp.png"}, {PieceType::B_PAWN, "pieces-png/bp.png"},
		{PieceType::W_ROOK, "pieces-png/wr.png"}, {PieceType::B_ROOK, "pieces-png/br.png"},
		{PieceType::W_KNIGHT, "pieces-png/wn.png"}, {PieceType::B_KNIGHT, "pieces-png/bn.png"},
		{PieceType::W_BISHOP, "pieces-png/wb.png"}, {PieceType::B_BISHOP, "pieces-png/bb.png"},
		{PieceType::W_QUEEN, "pieces-png/wq.png"}, {PieceType::B_QUEEN, "pieces-png/bq.png"},
		{PieceType::W_KING, "pieces-png/wk.png"}, {PieceType::B_KING, "pieces-png/bk.png"}
	};

	for (const auto& [piece, file] : textureFiles) {
		if (!pieceTextures[piece].loadFromFile(file)) {
			std::cerr << "Failed to load texture: " << file << std::endl;
			return false;
		}
		pieceTextures[piece].setSmooth(true); // Anti-aliasing for better visuals
	}
	return true;
}

/**
 * @brief  Renders the chessboard and all pieces.
 *
 * The selected piece is not drawn to allow smooth dragging.
 */
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

/**
 * @brief  Moves a piece and handles special cases like en passant & castling.
 */
void ChessBoard::movePiece(Square from, Square to) {
	Piece* movingPiece = board[from.row][from.col];
	PieceType type = movingPiece->getType();
	bool isWhite = movingPiece->isWhite();

	// Handle En Passant
	if (type == PieceType::W_PAWN || type == PieceType::B_PAWN) {
		if (to == enPassantTarget) {
			delete board[from.row][to.col]; // Capture en passant pawn
			board[from.row][to.col] = nullptr;
		}
	}

	// Handle Castling
	if (type == PieceType::W_KING || type == PieceType::B_KING) {
		bool canCastleKingSide = (isWhite && whiteKingCastle) || (!isWhite && blackKingCastle);
		bool canCastleQueenSide = (isWhite && whiteQueenCastle) || (!isWhite && blackQueenCastle);

		// Kingside Castling (g1/g8)
		if (to == Square{ from.row, 6 } && canCastleKingSide) {
			board[from.row][5] = board[from.row][7]; // Move the Rook to f1/f8
			board[from.row][7] = nullptr;
			board[from.row][5]->setSquare({ from.row, 5 });
			board[from.row][5]->setPosition({ 5 * SQUARE_SIZE, from.row * SQUARE_SIZE });
		}

		// Queenside Castling (c1/c8)
		else if (to == Square{ from.row, 2 } && canCastleQueenSide) {
			board[from.row][3] = board[from.row][0]; // Move the Rook to d1/d8
			board[from.row][0] = nullptr;
			board[from.row][3]->setSquare({ from.row, 3 });
			board[from.row][3]->setPosition({ 3 * SQUARE_SIZE, from.row * SQUARE_SIZE });
		}
	}

	updateCastleRights(movingPiece);

	delete board[to.row][to.col]; // Capture/Move the piece normally
	board[to.row][to.col] = movingPiece;
	board[from.row][from.col] = nullptr;

	movingPiece->setSquare(to);
	movingPiece->setPosition({ to.col * SQUARE_SIZE, to.row * SQUARE_SIZE });

	updateEnPassant(movingPiece, from, to);
}




/**
 * @brief  Filters out moves that leave the king in check.
 */
std::vector<Square> ChessBoard::getLegalMoves(Piece* piece) {
	std::vector<Square> legalMoves;
	std::vector<Square> moves = piece->getPossibleMoves();

	PieceType type = piece->getType();
	bool isWhite = piece->isWhite();
	int row = isWhite ? 7 : 0;

	// Handle En Passant 
	if (type == PieceType::W_PAWN || type == PieceType::B_PAWN) {
		int dir = piece->isWhite() ? -1 : 1;

		if (enPassantTarget.row == piece->getSquare().row + dir && abs(enPassantTarget.col - piece->getSquare().col) == 1) {
			moves.push_back(enPassantTarget); // Add en passant move
		}
	}

	// Handle Castling 
	else if (piece->getType() == PieceType::W_KING || piece->getType() == PieceType::B_KING) {
		bool canCastleKingSide = (isWhite && whiteKingCastle) || (!isWhite && blackKingCastle);
		bool canCastleQueenSide = (isWhite && whiteQueenCastle) || (!isWhite && blackQueenCastle);
		bool isChecked = isKingInCheck(isWhite);

		if (canCastleKingSide && !isChecked) {
			if (!board[row][5] && !board[row][6]) { // Squares f1/g1 or f8/g8 must be empty 
				moves.push_back({ row, 6 });
			}
		}

		if (canCastleQueenSide && !isChecked) {
			if (!board[row][1] && !board[row][2] && !board[row][3]) { // Squares b1/c1/d1 or b8/c8/d8 must be empty
				moves.push_back({ row, 2 }); // King moves to c1/c8
			}
		}
	}

	for (const Square& move : moves) {
		if (!doesMoveLeaveKingInCheck(piece->getSquare(), move)) {
			legalMoves.push_back(move);
		}
	}
	return legalMoves;
}

/**
 * @brief  Checks if checkmate happened.
 */
bool ChessBoard::isCheckMate(bool isWhite) {
	if (!isKingInCheck(isWhite)) return false;

	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			Piece* piece = getPiece({ row, col });
			if (!piece || piece->isWhite() != isWhite) continue;

			std::vector<Square> moves = getLegalMoves(piece);
			if (!moves.empty()) return false;
		}
	}
	return true;
}

/**
 * @brief  Simulates a move to determine if it exposes the king to check.
 *
 * The board state is temporarily modified, then restored to ensure no real
 * changes are made.
 *
 * TODO: Find out how to make this const. The board doest really change...
 */
bool ChessBoard::doesMoveLeaveKingInCheck(Square from, Square to) {
	Piece* movedPiece = getPiece(from);
	Piece* capturedPiece = getPiece(to);

	// Simulate move
	board[to.row][to.col] = movedPiece;
	board[from.row][from.col] = nullptr;
	movedPiece->setSquare(to);

	bool isInCheck = isKingInCheck(movedPiece->isWhite());

	// Undo move
	board[from.row][from.col] = movedPiece;
	board[to.row][to.col] = capturedPiece;
	movedPiece->setSquare(from);

	return isInCheck;
}

/**
 * @brief  Checks if the given player's king is under attack.
 *
 * Iterates over all opponent pieces and determines if any of them can legally
 * move to the king’s square.
 */
bool ChessBoard::isKingInCheck(bool isWhite) const {
	Square kingSquare = isWhite ? whiteKing->getSquare() : blackKing->getSquare();

	for (int row = 0; row < BOARD_SIZE; ++row) {
		for (int col = 0; col < BOARD_SIZE; ++col) {
			Piece* piece = getPiece({ row, col });
			if (!piece || piece->isWhite() == isWhite) continue;

			std::vector<Square> moves = piece->getPossibleMoves();
			if (std::find(moves.begin(), moves.end(), kingSquare) != moves.end()) {
				return true;
			}
		}
	}
	return false;
}


/**
 * @brief  Updates castling rights when a king or rook moves.
 *
 * If a king moves, both castling rights are revoked.
 * If a rook moves, only the corresponding castling right is revoked.
 */
void ChessBoard::updateCastleRights(Piece* piece) {
	int row = piece->getSquare().row;
	int col = piece->getSquare().col;

	switch (piece->getType()) {
	case PieceType::W_KING:
		whiteKingCastle = whiteQueenCastle = false;
		break;
	case PieceType::B_KING:
		blackKingCastle = blackQueenCastle = false;
		break;
	case PieceType::W_ROOK:
		if (row == 7) (col == 0) ? whiteQueenCastle = false : whiteKingCastle = false;
		break;
	case PieceType::B_ROOK:
		if (row == 0) (col == 0) ? blackQueenCastle = false : blackKingCastle = false;
		break;
	default:
		break;
	}
}


/**
 * @brief  Determines the en passant target square if applicable.
 */
void ChessBoard::updateEnPassant(Piece* movedPiece, Square from, Square to) {
	enPassantTarget = { -1, -1 }; // Reset by default

	int startRow = movedPiece->isWhite() ? 6 : 1;
	int targetRow = movedPiece->isWhite() ? 4 : 3;
	int enPassantRow = movedPiece->isWhite() ? 5 : 2;

	// Only applies to a double move from the starting position
	if (from.row == startRow && to.row == targetRow) {
		enPassantTarget = { enPassantRow, to.col };
	}
}

std::string ChessBoard::getCastlingRights() const {
	std::string rights;
	if (whiteKingCastle) rights += "K";
	if (whiteQueenCastle) rights += "Q";
	if (blackKingCastle) rights += "k";
	if (blackQueenCastle) rights += "q";
	return rights.empty() ? "-" : rights;
}

std::string ChessBoard::generateFEN(bool isWhiteTurn, int halfMoveClock, int fullMoveCount) const {
	return boardToFEN() + " " + (isWhiteTurn ? "w" : "b") + " " + getCastlingRights() + " " + (enPassantTarget == Square{-1,-1} ? "-" : squareToLiteral(enPassantTarget)) + " " + std::to_string(halfMoveClock) + " " + std::to_string(fullMoveCount);
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



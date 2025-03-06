#include "ChessBoard.h"

/**
 * @brief  Initializes the chessboard and loads textures.
 *
 * The board background is drawn once into a texture for performance optimization.
 * It also initializes castling rights and sets up the pieces.
 */
ChessBoard::ChessBoard() : boardTexture(sf::Vector2u(WINDOW_SIZE, WINDOW_SIZE)), boardSprite(boardTexture.getTexture()), whiteKing(nullptr), blackKing(nullptr), enPassantTarget({-1,-1}), whiteKingCastle(true), whiteQueenCastle(true), blackKingCastle(true), blackQueenCastle(true) {

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
	 * @brief  Moves a piece and handles special cases like en passant.
	 *
	 * If en passant is performed, the captured pawn is removed from its original
	 * position instead of the target square.
	 */
	void ChessBoard::movePiece(Square fromSquare, Square toSquare) {

		if (toSquare == enPassantTarget) {
			delete board[fromSquare.row][toSquare.col]; // Capture en passant pawn
			board[fromSquare.row][toSquare.col] = nullptr;
		}
		else {
			delete board[toSquare.row][toSquare.col]; // Capture the piece normally
		}
		board[toSquare.row][toSquare.col] = board[fromSquare.row][fromSquare.col];
		board[fromSquare.row][fromSquare.col] = nullptr;
		board[toSquare.row][toSquare.col]->setSquare(toSquare);
		board[toSquare.row][toSquare.col]->setPosition({ toSquare.col * SQUARE_SIZE, toSquare.row * SQUARE_SIZE });

		updateEnPassant(getPiece(toSquare), fromSquare, toSquare);
	}

	/**
	 * @brief  Filters out moves that leave the king in check.
	 */
	std::vector<Square> ChessBoard::getLegalMoves(Piece* piece) {
		std::vector<Square> moves = piece->getPossibleMoves(enPassantTarget);
		std::vector<Square> legalMoves;

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

				std::vector<Square> moves = piece->getPossibleMoves(enPassantTarget);
				if (std::find(moves.begin(), moves.end(), kingSquare) != moves.end()) {
					return true;
				}
			}
		}
		return false;
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

	/**
	 * @brief  Updates castling rights when a king or rook moves.
	 *
	 * If a king moves, both castling rights are revoked.
	 * If a rook moves, only the corresponding castling right is revoked.
	 */
	void ChessBoard::updateCastleRights(Piece* piece) {
		int fromRow = piece->getSquare().row;
		int fromCol = piece->getSquare().col;

		switch (piece->getType()) {
		case PieceType::W_KING:
			whiteKingCastle = false; whiteQueenCastle = false;
			break;
		case PieceType::B_KING:
			blackKingCastle = false; blackQueenCastle = false;
			break;
		case PieceType::W_ROOK:
			if (fromRow == 7) {
				if (fromCol == 0) whiteQueenCastle = false;
				else if (fromCol == 7) whiteKingCastle = false;
			}
			break;
		case PieceType::B_ROOK:
			if (fromRow == 0) {
				if (fromCol == 0) blackQueenCastle = false;
				else if (fromCol == 7) blackKingCastle = false;
			}
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

	

#include "Game.h"

/**
 * @brief  Initializes the game window and state variables.
 */
Game::Game() :
	window(sf::VideoMode({ 1200, 1200 }), "Chess Game", sf::Style::Close),
	isWhiteTurn(true),
	fullMoveCount(1),
	halfMoveClock(0),
	selectedPiece(nullptr),
	enPassantTarget("-")
{}

/**
 * @brief  Main game loop that runs until the window is closed.
 */
void Game::run() {
	bool isRunning = true;

	startStockfish();
	std::string testMove = "e2e4";
	std::cout << "Sending move: " << testMove << std::endl;
	sendMoveToStockfish(testMove);
	// Get Stockfish's response
	std::string stockfishMove = getStockfishMove();
	std::cout << "Stockfish's move: " << stockfishMove << std::endl;

	while (window.isOpen() && isRunning) {
		while (std::optional<sf::Event> event = window.pollEvent()) {
			handleEvents(event, isRunning);
		}

		window.clear();
		chessBoard.draw(window, selectedPiece);
		if (selectedPiece) {
			window.draw(*selectedPiece);
		}
		window.display();
	}
}

/**
 * @brief  Handles all SFML events such as closing the window and mouse interactions.
 */
void Game::handleEvents(const std::optional<sf::Event>& event, bool& isRuning) {
	if (event->is<sf::Event::Closed>()) {
		window.close();
	}
	else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
		if (mousePressed->button == sf::Mouse::Button::Left) {
			onPieceClicked(mousePressed);
		}
	}
	else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
		if (mouseReleased->button == sf::Mouse::Button::Left) {
			onPieceReleased(mouseReleased);
		}
	}
	else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
		if (selectedPiece) {
			selectedPiece->setPosition(sf::Vector2f(mouseMoved->position.x - dragOffset.x, mouseMoved->position.y - dragOffset.y));
		}
	}
}

/**
 * @brief  Handles piece selection when the player clicks on a piece.
 */
void Game::onPieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed) {
	int col = mouseButtonPressed->position.x / SQUARE_SIZE;
	int row = mouseButtonPressed->position.y / SQUARE_SIZE;

	if (auto piece = chessBoard.getPiece({ row, col }); piece && piece->isWhite() == isWhiteTurn) {
		selectedPiece = piece;		
		origianlPosition = selectedPiece->getPosition();
		dragOffset = { mouseButtonPressed->position.x - selectedPiece->getPosition().x, mouseButtonPressed->position.y - selectedPiece->getPosition().y };
		auto legalMoves = chessBoard.getLegalMoves(selectedPiece);
		std::cout << "Legal Moves:";
		for (const auto& move : legalMoves) {
			std::cout << chessBoard.squareToLiteral(move) << " ";
		}
		std::cout << "\n";
	}
}

/**
 * @brief  Handles piece movement when the player releases the mouse button.
 *
 * Validates the move and updates the game state, including en passant and castling.
 */
void Game::onPieceReleased(const sf::Event::MouseButtonReleased* mouseButtonReleased) {
	int newRow = mouseButtonReleased->position.y / SQUARE_SIZE;
	int newCol = mouseButtonReleased->position.x / SQUARE_SIZE;
	Square newSquare = { newRow, newCol };

	if (selectedPiece) {
		Square oldSquare = selectedPiece->getSquare();

		auto isValidMove = [&](const Square& pos) {
			auto legalMoves = chessBoard.getLegalMoves(selectedPiece);
			return std::find(legalMoves.begin(), legalMoves.end(), pos) != legalMoves.end();
			};

		if (isValidMove(newSquare)) {

			chessBoard.movePiece(oldSquare, newSquare);
			chessBoard.updateCastleRights(selectedPiece);

			
			if (!isWhiteTurn) {
				fullMoveCount++;
			}
			isWhiteTurn = !isWhiteTurn;
			if (chessBoard.isCheckMate(isWhiteTurn)) {
				std::cout << "Checkmate!";
			}
		}
		else {
			selectedPiece->setPosition(origianlPosition);
		}
	}

	selectedPiece = nullptr;
}

void Game::startStockfish() {
	stockfishProcess = _popen("stockfish.exe", "r"); // Open for reading only

	if (!stockfishProcess) {
		std::cerr << "Failed to start Stockfish! Check if stockfish.exe is in the same folder.\n";
		return;
	}

	char buffer[256];
	while (fgets(buffer, sizeof(buffer), stockfishProcess)) {
		std::string line(buffer);
		std::cout << "[Stockfish] " << line; // Print what Stockfish sends back
		if (line.find("uciok") != std::string::npos) {
			break; // Stockfish initialized successfully
		}
	}
}


void Game::sendMoveToStockfish(const std::string& move) {
	std::string command = "echo position startpos moves " + move + " | stockfish";
	std::system(command.c_str());

	std::string goCommand = "echo go | stockfish";
	std::system(goCommand.c_str());
}

std::string Game::getStockfishMove() {
	if (!stockfishProcess) return "";

	char buffer[128];
	std::string bestMove;

	while (fgets(buffer, sizeof(buffer), stockfishProcess)) {
		std::string line(buffer);
		if (line.find("bestmove") != std::string::npos) {
			bestMove = line.substr(9, 4); // Extract move in format "e2e4"
			break;
		}
	}

	return bestMove;
}

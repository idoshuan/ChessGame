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
	enPassantTarget("-"),
	stockfish("stockfish.exe")
{}

Game::~Game() {
	isStockfishRunning = false;  // Stop Stockfish from running
	if (stockfishThread.joinable()) {
		stockfishThread.join();  // Wait for Stockfish to finish
	}
}

/**
 * @brief  Main game loop that runs until the window is closed.
 */
void Game::run() {
	bool isRunning = true;

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

			if (!isWhiteTurn) {
				fullMoveCount++;
			}
			isWhiteTurn = !isWhiteTurn;
			if (chessBoard.isCheckMate(isWhiteTurn)) {
				std::cout << "Checkmate!";
			}
			else if (!isStockfishRunning) {
					isStockfishRunning = true;
					std::string fen = chessBoard.generateFEN(isWhiteTurn, halfMoveClock, fullMoveCount);
					stockfishThread = std::thread(&Game::runStockfish, this, fen, 3);
					stockfishThread.detach();  // Allow it to run independently
				}
			}
		}
		else {
			selectedPiece->setPosition(origianlPosition);
		}

	selectedPiece = nullptr;
}


void Game::runStockfish(const std::string& fen, int n) {
	std::vector<std::string> bestMoves = stockfish.getBestMoves(fen, n);

	std::cout << "Stockfish recommends:\n";
	for (const auto& move : bestMoves) {
		std::cout << move << std::endl;
	}

	isStockfishRunning = false;  // Allow new Stockfish calls
}










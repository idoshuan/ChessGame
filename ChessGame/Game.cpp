#include "Game.h"

Game::Game() :
	window(sf::VideoMode({ 1200, 1200 }), "Chess Game", sf::Style::Close),
	isWhiteTurn(true),
	fullMoveCount(1),
	halfMoveClock(0),
	selectedPiece(nullptr),
	enPassantTarget("-")
{
}


void Game::run() {

	while (window.isOpen()) {
		while (std::optional<sf::Event> event = window.pollEvent()) {
			handleEvents(event);
		}

		window.clear();
		chessBoard.draw(window, selectedPiece);
		if (selectedPiece) {
			window.draw(*selectedPiece);
		}
		window.display();
	}
}

void Game::handleEvents(const std::optional<sf::Event>& event) {
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

void Game::onPieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed) {
	int col = mouseButtonPressed->position.x / SQUARE_SIZE;
	int row = mouseButtonPressed->position.y / SQUARE_SIZE;

	if (selectedPiece = chessBoard.getPiece(row, col)) {
		origianlPosition = selectedPiece->getPosition();
		dragOffset = { mouseButtonPressed->position.x - selectedPiece->getPosition().x, mouseButtonPressed->position.y - selectedPiece->getPosition().y };
		auto legalMoves = selectedPiece->getLegalMoves(enPassantTarget);
		std::cout << "Legal Moves:";
		for (const auto& move : legalMoves) {
			std::cout << chessBoard.squareToLiteral(move) << " ";
		}
		std::cout << "\n";
	}
}

void Game::onPieceReleased(const sf::Event::MouseButtonReleased* mouseButtonReleased) {
	int newRow = mouseButtonReleased->position.y / SQUARE_SIZE;
	int newCol = mouseButtonReleased->position.x / SQUARE_SIZE;
	Square newSquare = { newRow, newCol };

	if (selectedPiece) {
		Square oldSquare = selectedPiece->getSquare();

		auto isValidMove = [&](const Square& pos) {
			auto legalMoves = selectedPiece->getLegalMoves(enPassantTarget);
			return std::find(legalMoves.begin(), legalMoves.end(), pos) != legalMoves.end();
			};

		if (isValidMove(newSquare)) {
			chessBoard.movePiece(oldSquare, newSquare);
			chessBoard.updateCastleRights(selectedPiece);
			if (selectedPiece->getType() == PieceType::W_PAWN || selectedPiece->getType() == PieceType::B_PAWN) {
				enPassantTarget = chessBoard.getEnPassantTarget(selectedPiece->isWhite(), oldSquare, newSquare);
			}
			if (!isWhiteTurn) {
				fullMoveCount++;
			}
			isWhiteTurn = !isWhiteTurn;
		}
		else {
			selectedPiece->setPosition(origianlPosition);
		}
		std::cout << "EnPassant Targets:" << enPassantTarget << "\n";
	}

	// Reset dragging variables
	selectedPiece = nullptr;
}


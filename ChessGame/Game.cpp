#include "Game.h"

Game::Game() :
	window(sf::VideoMode({ 1200, 1200 }), "Chess Game", sf::Style::Close),
	isWhiteTurn(true),
	whiteKingCastle(true), whiteQueenCastle(true), blackKingCastle(true), blackQueenCastle(true),
	enPassantTarget("-"),
	fullMoveCount(1),
	halfMoveClock(0),
	isDragging(false),
	selectedPiece(nullptr)
{
}


void Game::run() {

	while (window.isOpen()) {
		while (std::optional<sf::Event> event = window.pollEvent()) {
			handleEvents(event);
		}

		window.clear();
		chessBoard.draw(window, selectedPiece);
		if (isDragging && draggedSprite) {
			window.draw(*draggedSprite);
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
		if (mouseReleased->button == sf::Mouse::Button::Left && isDragging) {
			onPieceReleased(mouseReleased);
		}
	}
	else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
		if (isDragging && draggedSprite.has_value()) {
			draggedSprite->setPosition(sf::Vector2f(mouseMoved->position.x - dragOffset.x, mouseMoved->position.y - dragOffset.y));
		}
	}
}

void Game::onPieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed) {
	int col = mouseButtonPressed->position.x / SQUARE_SIZE;
	int row = mouseButtonPressed->position.y / SQUARE_SIZE;

	if ((selectedPiece = chessBoard.getPiece(row, col))) {
		isDragging = true;
		draggedSprite.emplace(chessBoard.getPieceTexture(selectedPiece));
		draggedSprite->setScale({ SQUARE_SIZE / draggedSprite->getTexture().getSize().x, SQUARE_SIZE / draggedSprite->getTexture().getSize().y });
		draggedSprite->setPosition({ col * SQUARE_SIZE, row * SQUARE_SIZE });

		dragOffset = { mouseButtonPressed->position.x - draggedSprite->getPosition().x, mouseButtonPressed->position.y - draggedSprite->getPosition().y };

		legalMoves = selectedPiece->getLegalMoves(enPassantTarget);
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
			return std::find(legalMoves.begin(), legalMoves.end(), pos) != legalMoves.end();
			};

		if (isValidMove(newSquare) && newSquare != oldSquare) {
			chessBoard.movePiece(oldSquare, newSquare);
			chessBoard.updateCastleRights(selectedPiece, whiteKingCastle, whiteQueenCastle, blackKingCastle, blackQueenCastle);
			if (selectedPiece->getType() == PieceType::W_PAWN || selectedPiece->getType() == PieceType::B_PAWN) {
				enPassantTarget = chessBoard.getEnPassantTarget(selectedPiece->isWhite(), oldSquare, newSquare);
			}
			if (!isWhiteTurn) fullMoveCount++;
			isWhiteTurn = !isWhiteTurn;
		}
		std::cout << "EnPassant Targets:" << enPassantTarget << "\n";
	}

	// Reset dragging variables
	draggedSprite.reset();
	isDragging = false;
	selectedPiece = nullptr;
}

std::string Game::getCastlingRights() const {
	std::string rights;
	if (whiteKingCastle) rights += "K";
	if (whiteQueenCastle) rights += "Q";
	if (blackKingCastle) rights += "k";
	if (blackQueenCastle) rights += "q";
	return rights.empty() ? "-" : rights;
}

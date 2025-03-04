#include "Game.h"

Game::Game() : window(sf::VideoMode({ 1200, 1200 }), "Chess Game", sf::Style::Close) {
	whiteToPlay = true;
	wk = wq = bk = bq = true;
	enPassantTarget = "-";
	fullMoveNumber = 1;
	halfMoveClock = 0;
	isDragging = false;
	selectedPiece = '.';
	selectedRow = selectedCol = -1;
}

void Game::run() {

	while (window.isOpen()) {
		while (std::optional<sf::Event> event = window.pollEvent()) {
			handleEvents(event);
		}

		window.clear();
		chessBoard.draw(window, selectedRow, selectedCol);
		if (isDragging && dragSprite.has_value()) {
			window.draw(*dragSprite);
		}
		window.display();
	}
}

void Game::handleEvents(std::optional<sf::Event>& event) {
	if (event->is<sf::Event::Closed>()) {
		window.close();
	}
	else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
		if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
			pieceClicked(mouseButtonPressed);
		}
	}
	else if (const auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
		if (mouseButtonReleased->button == sf::Mouse::Button::Left && isDragging) {
			pieceReleased(mouseButtonReleased);
		}
	}
	else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
		if (isDragging && dragSprite.has_value()) {
			dragSprite->setPosition(sf::Vector2f(mouseMoved->position.x - dragOffset.x, mouseMoved->position.y - dragOffset.y));
		}
	}
}

void Game::pieceClicked(const sf::Event::MouseButtonPressed* mouseButtonPressed) {

	int col = mouseButtonPressed->position.x / ChessBoard::getSquareSize();
	int row = mouseButtonPressed->position.y / ChessBoard::getSquareSize();

	if (chessBoard.getPiece(row, col) != '.') {
		isDragging = true;
		selectedPiece = chessBoard.getPiece(row, col);
		selectedRow = row;
		selectedCol = col;

		if (!dragSprite.has_value()) {
			dragSprite = sf::Sprite(chessBoard.getPieceTexture(selectedPiece));
		}
		dragSprite->setScale(sf::Vector2f(ChessBoard::getSquareSize() / dragSprite->getTexture().getSize().x, ChessBoard::getSquareSize() / dragSprite->getTexture().getSize().y));
		dragSprite->setPosition(sf::Vector2f(col * ChessBoard::getSquareSize(), row * ChessBoard::getSquareSize()));

		dragOffset.x = mouseButtonPressed->position.x - dragSprite->getPosition().x;
		dragOffset.y = mouseButtonPressed->position.y - dragSprite->getPosition().y;

		std::cout << chessBoard.generateFEN(castlingRightsToStr(), whiteToPlay, enPassantTarget, halfMoveClock, fullMoveNumber) << "\n";
	}
}

void Game::pieceReleased(const sf::Event::MouseButtonReleased* mouseButtonPressed) {
	int newRow = mouseButtonPressed->position.y / ChessBoard::getSquareSize();
	int newCol = mouseButtonPressed->position.x / ChessBoard::getSquareSize();

	if (selectedPiece != '.' && (newRow != selectedRow || newCol != selectedCol)) {
		chessBoard.movePiece(selectedRow, selectedCol, newRow, newCol);
		chessBoard.updateCastleRights(selectedPiece, selectedRow, selectedCol, wk, wq, bk, bq);
		enPassantTarget = chessBoard.getEnPassantTarget(selectedRow, newRow, newCol, selectedPiece);
		if (!whiteToPlay) fullMoveNumber++;
		whiteToPlay = !whiteToPlay;
	}

	// Reset dragging variables
	dragSprite.reset();
	isDragging = false;
	selectedPiece = '.';
	selectedRow = -1;
	selectedCol = -1;
}

std::string Game::castlingRightsToStr() {
	std::string rights = "";
	if (wk) rights += "K"; 
	if (wq) rights += "Q";
	if (bk) rights += "k";
	if (bq) rights += "q";
	return (rights.empty() ? "-" : rights);
}

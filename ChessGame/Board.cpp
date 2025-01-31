#include "Board.h"

Board::Board() {
	for (size_t row = 0; row < len; ++row) {
		std::vector<Square> currBoardRow;
		for (size_t col = 0; col < len; ++col) {
			currBoardRow.emplace_back(
				row, col, (row + col) % 2);  // if (row+col) % 2 == 1 then isBlack should be true
		}
		board.push_back(currBoardRow);
	}
}

void Board::draw(sf::RenderWindow& window) const {
	for (const auto& row : board) {
		for (const auto& square : row) {
			square.draw(window);
		}
	}
}

#include "ChessBoard.h"

const char ChessBoard::originalBoard[boardSize][boardSize] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};
const float ChessBoard::squareSize = ChessBoard::windowSize / ChessBoard::boardSize;


ChessBoard::ChessBoard() : boardTexture(sf::Vector2u(windowSize, windowSize)), boardSprite(boardTexture.getTexture()) {

    if (!loadTextures()) {
        std::cerr << "Error loading textures!" << std::endl;
    }

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            sf::RectangleShape square(sf::Vector2f(squareSize, squareSize));
            square.setPosition(sf::Vector2f(col * squareSize, row * squareSize));
            square.setFillColor((row + col) % 2 == 0 ? sf::Color::White : sf::Color(118, 150, 86));
            boardTexture.draw(square);
        }
    }
    memcpy(currBoard, originalBoard, sizeof(originalBoard));
    boardTexture.display();
    boardSprite.setTexture(boardTexture.getTexture());
}

bool ChessBoard::loadTextures() {
    std::map<char, std::string> textureFiles = {
        {'P', "pieces-png/wp.png"}, {'p', "pieces-png/bp.png"},
        {'R', "pieces-png/wr.png"}, {'r', "pieces-png/br.png"},
        {'N', "pieces-png/wn.png"}, {'n', "pieces-png/bn.png"},
        {'B', "pieces-png/wb.png"}, {'b', "pieces-png/bb.png"},
        {'Q', "pieces-png/wq.png"}, {'q', "pieces-png/bq.png"},
        {'K', "pieces-png/wk.png"}, {'k', "pieces-png/bk.png"}
    };

    for (auto& [piece, file] : textureFiles) {
        if (!pieceTextures[piece].loadFromFile(file)) {
            std::cerr << "Failed to load texture: " << file << std::endl;
            return false;
        }
        pieceTextures[piece].setSmooth(true);
    }
    return true;
}

void ChessBoard::draw(sf::RenderWindow& window, int selectedRow, int selectedCol) {
    window.draw(boardSprite);

    for (int row = 0; row < boardSize; ++row) {
        for (int col = 0; col < boardSize; ++col) {
            char piece = currBoard[row][col];
            if (piece != '.' && (row != selectedRow || col != selectedCol)) {
                sf::Sprite sprite(pieceTextures[piece]);
                sprite.setScale(sf::Vector2f(squareSize / sprite.getTexture().getSize().x, squareSize / sprite.getTexture().getSize().y));
                sprite.setPosition(sf::Vector2f(col * squareSize, row * squareSize));
                window.draw(sprite);
            }
        }
    }
}

void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    currBoard[toRow][toCol] = currBoard[fromRow][fromCol];
    currBoard[fromRow][fromCol] = '.';
    //std::cout << indexToLiteral(fromRow, fromCol) << indexToLiteral(toRow, toCol) << "\n";
}

std::vector<std::string> ChessBoard::getLegalMovesFromStockfish(const std::string& fen) {
    std::vector<std::string> legalMoves;

    // Start Stockfish process
    FILE* stockfish = _popen("stockfish.exe", "w+");
    if (!stockfish) {
        std::cerr << "Failed to start Stockfish!" << std::endl;
        return legalMoves;
    }

    // Send UCI initialization commands
    fprintf(stockfish, "uci\n");
    fflush(stockfish);

    // Set the position in FEN
    fprintf(stockfish, "position fen %s\n", fen.c_str());
    fflush(stockfish);

    // Ask Stockfish for legal moves
    fprintf(stockfish, "go perft 1\n"); // Returns all possible legal moves at depth 1
    fflush(stockfish);

    // Read Stockfish's output
    std::string fullOutput;
    char buffer[1024];  // Bigger buffer to avoid truncation

    while (fgets(buffer, sizeof(buffer), stockfish) != nullptr) {
        fullOutput += buffer;  // Store full response
    }

    // Close Stockfish process
    _pclose(stockfish);

    // Parse moves from the output
    std::istringstream iss(fullOutput);
    std::string word;

    while (iss >> word) {
        if (word == "Legal" || word == "moves:") {
            continue;
        }
        else if (word.length() >= 4 && word.length() <= 5) {
            legalMoves.push_back(word);  // Store only valid moves
        }
    }

    return legalMoves;
}

std::string ChessBoard::generateFEN(std::string castlingRights, bool whiteToPlay, std::string enPassant, int halfmoveClock, int fullmoveNumber) {

    std::string fen = boardToFEN();  // Get board representation

    fen += " " + std::string(whiteToPlay ? "w" : "b");
    fen += " " + (castlingRights.empty() ? "-" : castlingRights);  // Castling rights
    fen += " " + (enPassant.empty() ? "-" : enPassant);  // En passant target square
    fen += " " + std::to_string(halfmoveClock);  // Halfmove clock
    fen += " " + std::to_string(fullmoveNumber);  // Fullmove number

    return fen;
}

std::string ChessBoard::boardToFEN() {
    std::string fen = "";

    for (int row = 0; row < 8; row++) {
        int emptyCount = 0; // Count empty squares
        for (int col = 0; col < 8; col++) {
            char piece = currBoard[row][col];

            if (piece == '.') { // Empty square
                emptyCount++;
            }
            else {
                if (emptyCount > 0) {
                    fen += std::to_string(emptyCount); // Add empty count before placing a piece
                    emptyCount = 0;
                }
                fen += piece; // Add piece character (e.g., 'r')
            }
        }
        if (emptyCount > 0) {
            fen += std::to_string(emptyCount); // Add any remaining empty squares
        }
        if (row < 7) fen += "/"; // Separate ranks
    }

    return fen;
}

void ChessBoard::updateCastleRights(char piece, int fromRow, int fromCol, bool& wk, bool& wq, bool& bk, bool& bq) {
    if (piece == 'K') { wk = false; wq = false; } 
    else if (piece == 'k') { bk = false; bq = false; } 
    else if (piece == 'R' && fromRow == 7) {
        if (fromCol == 0) wq = false; 
        else if (fromCol == 7) wk = false; 
    }
    else if (piece == 'r' && fromRow == 0) {
        if (fromCol == 0) bq = false; 
        else if (fromCol == 7) bk = false; 
    }
    //std::cout << wk << wq << bk << bq << "\n";
}

std::string ChessBoard::getEnPassantTarget(int fromRow, int toRow, int col, char piece) {
    if (piece == 'P' && fromRow == 6 && toRow == 4) { // White pawn double move
        return std::string(1, 'a' + col) + "3"; 
    }
    if (piece == 'p' && fromRow == 1 && toRow == 3) { // Black pawn double move
        return std::string(1, 'a' + col) + "6"; 
    }
    return "-"; // No en passant
}
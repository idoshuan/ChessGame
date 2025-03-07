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
	auto stockfishMove = getStockfishMoves();
	for (auto& move : stockfishMove) {
		std::cout << move << " ";
	}
	std::cout << "\n";

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
HANDLE stockfishInput = nullptr;  // For sending commands
HANDLE stockfishOutput = nullptr; // For reading responses

void createPipe(HANDLE& readPipe, HANDLE& writePipe) {
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;  // Explicitly use nullptr
	if (!CreatePipe(&readPipe, &writePipe, &saAttr, 0)) {
		std::cerr << "Failed to create pipes.\n";
		exit(EXIT_FAILURE);
	}
	SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);
}

void Game::startStockfish() {
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;

	HANDLE hReadPipe, hWritePipe;
	HANDLE hReadPipeIn, hWritePipeIn; // For writing to Stockfish

	// Create pipes for Stockfish input and output
	if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0) ||
		!CreatePipe(&hReadPipeIn, &hWritePipeIn, &saAttr, 0)) {
		std::cerr << "Failed to create pipes.\n";
		return;
	}

	// Prevent inheritance of the read end of the output pipe and the write end of the input pipe
	SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(hWritePipeIn, HANDLE_FLAG_INHERIT, 0);

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFOA siStartInfo = { sizeof(STARTUPINFOA) };

	siStartInfo.hStdError = hWritePipe;
	siStartInfo.hStdOutput = hWritePipe;
	siStartInfo.hStdInput = hReadPipeIn; // Redirect Stockfish's input to the read end of our input pipe
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	std::string command = "stockfish.exe";
	std::vector<char> cmdVec(command.begin(), command.end()); // CreateProcessA() requires a modifiable char* because Windows might modify the command string internally.
	cmdVec.push_back('\0'); 

	if (!CreateProcessA(NULL, cmdVec.data(), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
		std::cerr << "Failed to start Stockfish! Ensure stockfish.exe is in the same directory.\n";
		return;
	}

	// Close unnecessary handles
	CloseHandle(piProcInfo.hThread);
	CloseHandle(hWritePipe);    // Close Stockfish's output write pipe
	CloseHandle(hReadPipeIn);   // Close Stockfish's input read pipe

	// Assign pipes for input and output
	stockfishOutput = hReadPipe;   // Correctly assign the output pipe for reading
	stockfishInput = hWritePipeIn; // Correctly assign the input pipe for writing

	std::cout << "Stockfish started successfully!\n";
}


void Game::sendMoveToStockfish(const std::string& move) {
	if (!stockfishInput) return;

	Sleep(100);  // Give Stockfish time to initialize

	std::string command = "position startpos moves " + move + "\n";
	command += "go movetime 1000\n";

	DWORD written;
	if (!WriteFile(stockfishInput, command.c_str(), command.size(), &written, NULL)) {
		std::cerr << "Failed to send command to Stockfish.\n";
		return;
	}

	FlushFileBuffers(stockfishInput); // Ensure Stockfish receives the command immediately
}


std::vector<std::string> Game::getStockfishMoves(int moveCount) {
	if (!stockfishOutput) return {};

	std::vector<std::string> bestMoves;
	std::string output;
	char buffer[256];
	DWORD bytesRead;
	int attempts = 0;
	const int maxAttempts = 50;

	// **Request multiple best moves from Stockfish**
	std::string command = "setoption name MultiPV value " + std::to_string(moveCount) + "\n";
	command += "go movetime 1000\n";

	DWORD written;
	WriteFile(stockfishInput, command.c_str(), command.size(), &written, NULL);
	FlushFileBuffers(stockfishInput);

	while (attempts++ < maxAttempts) {
		Sleep(100);  // Allow Stockfish time to respond

		if (!PeekNamedPipe(stockfishOutput, NULL, 0, NULL, &bytesRead, NULL) || bytesRead == 0)
			continue;

		// Read available data from the pipe
		if (ReadFile(stockfishOutput, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
			buffer[bytesRead] = '\0';  // Null-terminate the buffer
			output += buffer;

			// Extract moves from "info ... pv"
			std::istringstream ss(output);
			std::string token;
			while (ss >> token) {
				if (token == "pv") { // Look for "pv" which contains the best move sequence
					std::string move;
					ss >> move; // Read the first move in the PV line
					if (!move.empty() && bestMoves.size() < moveCount) {
						bestMoves.push_back(move);
					}
				}
			}

			// Stop if we got the required number of moves
			if (bestMoves.size() >= moveCount) break;
		}
	}

	if (bestMoves.empty()) {
		std::cerr << "Stockfish did not respond in time or didn't return enough moves!\n";
	}

	return bestMoves;
}



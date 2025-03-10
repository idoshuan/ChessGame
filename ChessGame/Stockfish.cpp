#include "Stockfish.h"


Stockfish::Stockfish(const std::string& path) : stockfishPath(path) {
    startStockfish();
}

Stockfish::~Stockfish() {
    sendCommand("quit");
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
    CloseHandle(hChildStd_IN_Wr);
    CloseHandle(hChildStd_OUT_Rd);
}
#include <windows.h>
#include <iostream>
#include <vector>

void Stockfish::startStockfish() {
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0);
    CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0);

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Convert std::string to std::wstring using MultiByteToWideChar
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, stockfishPath.c_str(), -1, NULL, 0);
    std::wstring wStockfishPath(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, stockfishPath.c_str(), -1, &wStockfishPath[0], size_needed);

    // Use CreateProcessW with wide-character string
    if (!CreateProcessW(wStockfishPath.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        std::cerr << "Failed to start Stockfish!" << std::endl;
        return;
    }

    sendCommand("uci");
    sendCommand("isready");
}



void Stockfish::sendCommand(const std::string& command) {
    std::string cmd = command + "\n";
    DWORD written;
    WriteFile(hChildStd_IN_Wr, cmd.c_str(), cmd.length(), &written, NULL);
}

std::string Stockfish::readOutput() {
    DWORD bytesRead;
    CHAR buffer[4096];
    std::string output;

    while (true) {
        ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (bytesRead == 0) break;
        buffer[bytesRead] = '\0';
        output += buffer;
        if (output.find("bestmove") != std::string::npos) break;
    }

    return output;
}

std::vector<std::string> Stockfish::getBestMoves(const std::string& fen, int n) {
    sendCommand("uci");       // Ensure Stockfish is initialized
    sendCommand("isready");   // Ensure it's ready before sending a new position
    sendCommand("setoption name MultiPV value " + std::to_string(n));  // Set MultiPV explicitly
    sendCommand("position fen " + fen);
    sendCommand("go depth 20");  // No need for multipv here since it's set globally

    std::string output = readOutput();
    std::unordered_map<int, std::string> bestMovesMap;  // Store moves by their multipv index
    std::istringstream iss(output);
    std::string line;
    int highestDepth = 0;

    while (std::getline(iss, line)) {
        if (line.find("info depth") != std::string::npos && line.find(" multipv ") != std::string::npos) {
            std::istringstream lineStream(line);
            std::string token;
            int depth = 0, multipv = 0;
            std::string move;

            while (lineStream >> token) {
                if (token == "depth") {
                    lineStream >> depth;
                }
                else if (token == "multipv") {
                    lineStream >> multipv;
                }
                else if (token == "pv") {
                    lineStream >> move;
                    break;
                }
            }

            if (!move.empty() && multipv > 0 && multipv <= n) {
                bestMovesMap[multipv] = move;  // Store move indexed by multipv
                highestDepth = std::max<int>(highestDepth, depth);
            }
        }
    }

    // Ensure only `N` moves are returned, sorted by multipv order
    std::vector<std::string> bestMoves;
    for (int i = 1; i <= n; i++) {
        if (bestMovesMap.find(i) != bestMovesMap.end()) {
            bestMoves.push_back(bestMovesMap[i]);
        }
    }

    return bestMoves;
}






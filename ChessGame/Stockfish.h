#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <sstream>
#include <unordered_map>


class Stockfish {
private:
    std::string stockfishPath;
    HANDLE hChildStd_IN_Rd = NULL, hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL, hChildStd_OUT_Wr = NULL;
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;

    void startStockfish();
    void sendCommand(const std::string& command);
    std::string readOutput();

public:
    Stockfish(const std::string& path);
    ~Stockfish();
    std::vector<std::string> getBestMoves(const std::string& fen, int n);
};


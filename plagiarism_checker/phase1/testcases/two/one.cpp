#include <vector>
#include <string>
#include <iostream>

class Solution {
public:
    std::vector<std::vector<std::string>> solveNQueens(int size) {
        std::vector<std::vector<std::string>> solutions;
        std::vector<int> positions(size, -1); // Store the column positions of queens
        backtrack(size, 0, positions, solutions);
        return solutions;
    }

private:
    void backtrack(int size, int currentRow, std::vector<int>& positions, std::vector<std::vector<std::string>>& solutions) {
        if (currentRow == size) {
            addSolution(solutions, positions, size);
            return;
        }

        for (int col = 0; col < size; col++) {
            if (canPlaceQueen(currentRow, col, positions)) {
                positions[currentRow] = col; // Place the queen
                backtrack(size, currentRow + 1, positions, solutions); // Recur to place next queen
                positions[currentRow] = -1; // Remove the queen (backtrack)
            }
        }
    }

    bool canPlaceQueen(int row, int col, const std::vector<int>& positions) {
        for (int r = 0; r < row; r++) {
            int c = positions[r];
            if (c == col || abs(c - col) == abs(r - row)) {
                return false;
            }
        }
        return true;
    }

    void addSolution(std::vector<std::vector<std::string>>& solutions, const std::vector<int>& positions, int size) {
        std::vector<std::string> board(size, std::string(size, '.'));
        for (int r = 0; r < size; r++) {
            board[r][positions[r]] = 'Q'; // Place the queen on the board
        }
        solutions.push_back(board);
    }
};

int main() {
    Solution solver;

    int n;
    std::cin >> n;

    if (n < 1 || n > 9) {
        std::cout << "Invalid input. n must be between 1 and 9." << std::endl;
        return 1;
    }

    std::vector<std::vector<std::string>> results = solver.solveNQueens(n);

    std::cout << "[";
    for (size_t i = 0; i < results.size(); i++) {
        std::cout << "[";
        for (size_t j = 0; j < results[i].size(); j++) {
            std::cout << "\"" << results[i][j] << "\"";
            if (j != results[i].size() - 1) std::cout << ",";
        }
        std::cout << "]";
        if (i != results.size() - 1) std::cout << ",";
    }
    std::cout << "]" << std::endl;

    return 0;
}
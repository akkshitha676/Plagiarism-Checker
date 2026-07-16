#include <vector>
#include <string>
#include <iostream>

class Solution {
public:
    std::vector<std::vector<std::string>> solveNQueens(int n) {
        std::vector<std::vector<std::string>> results;
        if (n == 1) {
            results.push_back({"Q"});
            return results;
        }
        if (n == 2 || n == 3) {
            return results; // No solutions for n = 2 or 3
        }

        std::vector<int> solution(n, -1); // Initialize the solution vector
        solveNQueensRec(n, solution, 0, results);
        return results;
    }

private:
    void solveNQueensRec(int n, std::vector<int>& solution, int row,
                         std::vector<std::vector<std::string>>& results) {
        if (row == n) {
            // If we've placed all queens, construct the board and add to results
            std::vector<std::string> solutionStr = constructSolutionString(solution);
            results.push_back(solutionStr);
            return;
        }

        for (int col = 0; col < n; col++) {
            if (isValidMove(row, col, solution)) {
                solution[row] = col; // Place the queen
                solveNQueensRec(n, solution, row + 1, results); // Recur for the next row
                solution[row] = -1; // Backtrack
            }
        }
    }

    bool isValidMove(int proposedRow, int proposedCol,
                     const std::vector<int>& solution) {
        for (int i = 0; i < proposedRow; i++) {
            int oldRow = i;
            int oldCol = solution[i];
            int diagonalOffset = proposedRow - oldRow;

            // Check if queens are in the same column or diagonals
            if (oldCol == proposedCol ||
                oldCol == proposedCol - diagonalOffset ||
                oldCol == proposedCol + diagonalOffset) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::string> constructSolutionString(const std::vector<int>& solution) {
        std::vector<std::string> returnArr(solution.size(), std::string(solution.size(), '.'));
        for (int i = 0; i < solution.size(); i++) {
            returnArr[i][solution[i]] = 'Q'; // Place the queen on the board
        }
        return returnArr;
    }
};

int main() {
    Solution solution;

    int n;
    std::cin >> n;

    if (n < 1 || n > 9) {
        std::cout << "Invalid input. n must be between 1 and 9." << std::endl;
        return 1;
    }

    std::vector<std::vector<std::string>> results = solution.solveNQueens(n);

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
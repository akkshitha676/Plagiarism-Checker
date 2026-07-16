#include <vector>
#include <string>
#include <map>

class Solution {
public:
    std::map<int, int> straightline;
    std::map<int, int> diagonallyleft;
    std::map<int, int> diagonallyright;

    std::vector<std::vector<std::string>> solveNQueens(int n) {
        std::vector<std::vector<std::string>> output;
        std::vector<int> path;
        solve(n, 0, output, path);
        return output;
    }

    void solve(int& n, int row, std::vector<std::vector<std::string>>& output, std::vector<int> path) {
        if (row == n) {
            make_ans(output, path, n);
            return;
        }
        for (int i = 0; i < n; i++) {
            if (isSafe(row, i, n)) {
                path.push_back(i);
                straightline[i]++;
                diagonallyright[row + i]++;
                diagonallyleft[row - i]++;
                solve(n, row + 1, output, path);
                straightline[i]--;
                diagonallyright[row + i]--;
                diagonallyleft[row - i]--;
                path.pop_back();
            }
        }
        return;
    }

    bool isSafe(int row, int col, int n) {
        if (straightline[col] || diagonallyright[row + col] || diagonallyleft[row - col]) {
            return false;
        }
        return true;
    }

    void make_ans(std::vector<std::vector<std::string>>& output, std::vector<int> path, int n) {
        std::vector<std::string> ans;
        for (auto i : path) {
            std::string temp(n, '.');
            temp[i] = 'Q';
            ans.push_back(temp);
        }
        output.push_back(ans);
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
#include<bits/stdc++.h>
using namespace std;

int SEED = 42;
mt19937 rng(SEED);

int randomInt(int low, int high) {
    uniform_int_distribution<> dist(low, high);
    return dist(rng);
}

double randomDouble() {
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

int getCost(const vector<vector<int>>& grid) {
    int cost = 0;
    for (int i = 0; i < 9; i++) {
        set<int> rowSet, columnSet;
        for (int j = 0; j < 9; j++) {
            rowSet.insert(grid[i][j]);
            columnSet.insert(grid[j][i]);
        }
        cost += (18 - (rowSet.size() + columnSet.size()));
    }
    return cost;
}

void printGrid(const vector<vector<int>>& grid) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void populateGrid(vector<vector<int>>& grid, vector<vector<bool>>& fixed) {
    vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    shuffle(numbers.begin(), numbers.end(), rng);

    for (int rowBox = 0; rowBox < 3; rowBox++) {
        for (int colBox = 0; colBox < 3; colBox++) {
            vector<int> boxNumbers = numbers;

            for (int i = 3 * rowBox; i < 3 * (rowBox + 1); i++) {
                for (int j = 3 * colBox; j < 3 * (colBox + 1); j++) {
                    if (fixed[i][j]) {
                        boxNumbers.erase(remove(boxNumbers.begin(), boxNumbers.end(), grid[i][j]), boxNumbers.end());
                    }
                }
            }

            int idx = 0;
            for (int i = 3 * rowBox; i < 3 * (rowBox + 1); i++) {
                for (int j = 3 * colBox; j < 3 * (colBox + 1); j++) {
                    if (!fixed[i][j]) {
                        grid[i][j] = boxNumbers[idx];
                        idx++;
                    }
                }
            }
        }
    }
}



void swapUnfixed(vector<vector<int>>& grid, vector<vector<bool>>& fixed) {
    int rowBox = randomInt(0, 2);
    int colBox = randomInt(0, 2);
    int i1 = 0, j1 = 0;
    while (true) {
        i1 = 3 * rowBox + randomInt(0, 2);
        j1 = 3 * colBox + randomInt(0, 2);
        if (!fixed[i1][j1]) {
            break;
        }
    }
    int i2 = 0, j2 = 0;
    while (true) {
        i2 = 3 * rowBox + randomInt(0, 2);
        j2 = 3 * colBox + randomInt(0, 2);
        if (!fixed[i2][j2]) {
            break;
        }
    }

    swap(grid[i1][j1], grid[i2][j2]);
}

double stddev(const vector<int>& costs) {
    int size = costs.size();
    double mean = 0;
    for (int i : costs) {
        mean += i;
    }
    mean = mean / size;
    double var = 0;
    for (int i : costs) {
        var += (i - mean) * (i - mean);
    }
    return sqrt(var / size);
}

int main() {
    vector<vector<int>> inputGrid = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    vector<vector<int>> grid = inputGrid;
    vector<vector<bool>> fixed(9, vector<bool>(9, false));

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                fixed[i][j] = true;
            }
        }
    }

    cout << "Input Grid:\n";
    printGrid(inputGrid);

    populateGrid(grid, fixed);

    cout << "Populated Grid:\n";
    printGrid(grid);

    vector<vector<int>> tempGrid;
    vector<int> costs;
    tempGrid = grid;
    for (int i = 0; i < 20; i++) {
        swapUnfixed(tempGrid, fixed);
        costs.push_back(getCost(tempGrid));
    }

    int initialCost = getCost(grid);
    double initialTemp = stddev(costs);
    vector<vector<int>> currGrid = grid;
    int reheatPeriod = 200;
    queue<int> checkReheat;
    int iterations = 0;
    int maxIterations = 40000;

    while (true) {
        if (iterations > maxIterations) {
            break;
        }

        if (initialCost == 0) {
            cout << "Perfect Solution!\n\n";
            break;
        }

        if (iterations < reheatPeriod) {
            checkReheat.push(initialCost);
        }
        else {
            if (initialCost > checkReheat.front()) {
                initialCost = getCost(grid);
                initialTemp = stddev(costs);
                currGrid = inputGrid;
                populateGrid(currGrid, fixed);

                cout << "Reheated to get new Populated Grid:\n";
                printGrid(currGrid);

                checkReheat = queue<int>();
                iterations = 0;
            }
            else {
                checkReheat.pop();
                checkReheat.push(initialCost);
            }
        }

        tempGrid = currGrid;
        swapUnfixed(tempGrid, fixed);
        int tempCost = getCost(tempGrid);
        double delta = tempCost - initialCost;

        if (delta < 0 || exp(-delta / initialTemp) > randomDouble()) {
            currGrid = tempGrid;
            initialCost = tempCost;
        }

        initialTemp *= 0.99;
        iterations++;
    }

    cout << "Final Grid:\n";
    printGrid(currGrid);
    cout << "Cost = " << initialCost << "\n";

    return 0;
}

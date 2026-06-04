#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <limits.h>

// MAIN VARIABLES:

unsigned long long _N;  // Number of aminoacids in the sequence
std::vector<unsigned long long> _P; // Aminoacid potencials sequence
std::vector<unsigned long long> _C; // Aminoacid classes sequence

const unsigned long long _AFFINITY[5][5] = { // Affinity matrix for aminoacid class combinations
    // P  N  A  B  T
    { 1, 3, 1, 3, 1 }, // P
    { 5, 1, 0, 1, 1 }, // N
    { 0, 1, 0, 4, 1 }, // A
    { 1, 3, 2, 3, 1 }, // B
    { 1, 1, 1, 1, 1 }  // T (Terminal)
};

struct DPCell {
    unsigned long long energy; // energy freed by removing choice last
    unsigned long long choice; // last removed aminoacid index
    unsigned long long leftL, leftR; // next left interval in the path
    unsigned long long rightL, rightR; // next right interval in the path
};

std::vector<std::vector<unsigned long long>> leftEnergy; // Energy freed in [l,k]
std::vector<std::vector<unsigned long long>> rightEnergy; // Energy freed in [k,r]

std::vector<std::vector<DPCell>> dpTable; // DP table storing information for each interval

const unsigned long long ULL_NONE = ULLONG_MAX; // null / -1 marker
//---------------------------------------------------------------

/**
 * Reads the aminoacid sequence information and the sequence itself
 * from standard input.
 */
void readSequence() {
	std::cin >> _N;

    // vectors hold 1 to N aminoacids, terminals not stored, index 0 ignored
	_P.resize(_N+1); _C.resize(_N+1);
    _C[0] = 4; // terminal; for energy calculation

	for (unsigned long long i = 1; i <= _N; ++i) std::cin >> _P[i]; // read potentials
    std::cin.get(); // skip newline
    // read sequence and map to class index
    for (unsigned long long i = 1; i <= _N; ++i) switch (std::cin.get()) { 
            case 'P': _C[i] = 0; break;
            case 'N': _C[i] = 1; break;
            case 'A': _C[i] = 2; break;
            case 'B': _C[i] = 3; break;
            default: _C[i] = ULL_NONE; break; // error
    }    
}

//---------------------------------------------------------------

/**
 * Computes the freed energies by removing aminoacid k last from every interval [l, r].
 * 
 * E = P(i-1) * Af(c(i-1), c(i)) * P(i) + P(i) * Af(c(i), c(i+1)) * P(i+1)
 * 
 * Since aminoacid k is removed last, its neighbors are always l-1 and r+1 if they exist, otherwise the terminals (pot = 1, class = _C[0]).
 */
void computeEnergies() {
	leftEnergy.assign(_N+2, std::vector<unsigned long long>(_N+2, 0));
    rightEnergy.assign(_N+2, std::vector<unsigned long long>(_N+2, 0));

    for (unsigned long long l = 1; l <= _N; ++l) {
        for (unsigned long long k = l; k <= _N; ++k) {
            unsigned long long l_pot   = (l > 1) ? _P[l-1] : 1;
            unsigned long long l_class = (l > 1) ? _C[l-1] : _C[0];
            leftEnergy[l][k] = l_pot * _AFFINITY[l_class][_C[k]] * _P[k];
        }
    }

    for (unsigned long long k = 1; k <= _N; ++k) {
        for (unsigned long long r = k; r <= _N; ++r) {
            unsigned long long r_pot   = (r < _N) ? _P[r+1] : 1;
            unsigned long long r_class = (r < _N) ? _C[r+1] : _C[0];
            rightEnergy[k][r] = _P[k] * _AFFINITY[_C[k]][r_class] * r_pot;
        }
    }
}

//---------------------------------------------------------------

/**
 * Builds the removal path from a given DPCell by backtracking through the stored intervals.
 * 
 * @param cell DPCell from which to start building the path
 * @param path Vector to store the removal order of aminoacids
 */
void buildPath(DPCell cell, std::vector<unsigned long long>& path) {
    std::stack<std::pair<unsigned long long, unsigned long long>> intervals;
    std::pair<unsigned long long, unsigned long long> top;
    intervals.push({cell.leftL, cell.leftR});
    intervals.push({cell.rightL, cell.rightR});
    intervals.push({ULL_NONE-1, cell.choice}); // marker for root

    while (!intervals.empty()) {
        top = intervals.top(); intervals.pop();
        unsigned long long l = top.first, r = top.second;

        if (l == ULL_NONE || r == ULL_NONE) continue;
        if (l == ULL_NONE-1) { path.push_back(r); continue; } // root marker

        DPCell& sub = dpTable[l][r];
        // left - right - root
        intervals.push({sub.leftL, sub.leftR});
        intervals.push({sub.rightL, sub.rightR});
        intervals.push({ULL_NONE-1, sub.choice});
    }
    std::reverse(path.begin(), path.end());
}

//---------------------------------------------------------------

/**
 * Solves max freed energy by removing aminoacids in optimal order using dynamic programming.
 * 
 * Finds best energy for the subinterval from l to r inclusive.
 * In this case, always solves for 1 to N.
 * 
 * dpTable[l][r] holds the best DPCell for interval [l, r].
 * 
 * In other words, the best outcome of removing all aminoacids in [l, r], with k being the last removed.
 * 
 * Complexity: O(N^3)
 * 
 * @return DPCell containing max freed energy and last removal choice for the full sequence
 */
DPCell solveMaxEnergy() {
    dpTable.assign(_N+1, std::vector<DPCell>(_N+1)); // initialize DP table

    // base cases: single element intervals
    for (unsigned long long i = 1; i <= _N; ++i) {
        unsigned long long energy = leftEnergy[i][i] + rightEnergy[i][i];
        dpTable[i][i] = DPCell{energy, i, ULL_NONE, ULL_NONE, ULL_NONE, ULL_NONE};
    }

    std::vector<unsigned long long> pathCurr, pathBest;

    // build up by interval length
    for (unsigned long long len = 2; len <= _N; ++len) {
        for (unsigned long long l = 1; l+len-1 <= _N; ++l) {
            unsigned long long r = l+len-1;
            DPCell best = DPCell{0, ULL_NONE, ULL_NONE, ULL_NONE, ULL_NONE, ULL_NONE};
            
            for (unsigned long long k = l; k <= r; ++k) {
                unsigned long long leftIntervalEnergy = (k > l) ? dpTable[l][k-1].energy : 0;
                unsigned long long rightIntervalEnergy = (k < r) ? dpTable[k+1][r].energy : 0;
                unsigned long long currEnergy = leftIntervalEnergy + rightIntervalEnergy + (leftEnergy[l][k] + rightEnergy[k][r]); // left and right interval energies + k

                // Determine next indices for path reconstruction
                unsigned long long leftL = (k > l) ? l : -1, leftR = (k > l) ? k - 1 : ULL_NONE;
                unsigned long long rightL = (k < r) ? k + 1 : -1, rightR = (k < r) ? r : ULL_NONE;                

                DPCell currCell = DPCell{currEnergy, k, leftL, leftR, rightL, rightR};
                
                // Choose the best energy; in case of tie, the lexicographically smallest path
                if (currEnergy >= best.energy) best = currCell;
                else if (currEnergy == best.energy) {
                    pathCurr.clear(), pathBest.clear();
                    buildPath(currCell, pathCurr), buildPath(best, pathBest);
                    if (pathCurr < pathBest) best = currCell;
                }
            }
            dpTable[l][r] = best;
        }
    }
    return dpTable[1][_N];
}

//---------------------------------------------------------------

int main() {
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

	// Read the aminoacid sequence info and the sequence itself
	readSequence();

    // Compute all k aminoacid removal energies
    computeEnergies();

	// Output max freed energy and corresponding sequence
	DPCell result = solveMaxEnergy();
    std::cout << result.energy << std::endl;
    std::vector<unsigned long long> removalPath;
    buildPath(result, removalPath);
    for (unsigned long long i = 0; i < removalPath.size(); ++i)
        std::cout << removalPath[i] << (i + 1 < removalPath.size() ? " " : "");
    std::cout << std::endl;

	return 0;
}

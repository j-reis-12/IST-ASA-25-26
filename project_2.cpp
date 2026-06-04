#include <iostream>
#include <vector>

// MAIN VARIABLES:

unsigned int _N; // Number of crossings (>=2)
unsigned int _M; // Number of trucks (>=2)
unsigned int _m1, _m2; // Truck number range to consider
unsigned int _K; // Number of direct connections between crossings (>=1)

std::vector<std::vector<unsigned int>> _adj; // Adjacency list for crossings
std::vector<unsigned int> _topo_order; // Topological order of crossings
std::vector<unsigned int> _topo_pos; // Position of each node in the topological order

std::vector<unsigned int> dp; // DP array for counting paths
std::vector<char> _reachable; // Reachability marker

// List of (A,B) paths for each t
std::vector<std::vector<std::pair<unsigned int, unsigned int>>> _truck_routes; 

//---------------------------------------------------------------

/**
 * Reads all the map information from standard input.
 * 
 * Complexity: O(K) where K is the number of direct connections.
 */
void readMapInfo() {
	std::cin >> _N >> _M >> _m1 >> _m2 >> _K;
    _adj.resize(_N);
    for (int i = 0; i < (int)_K; ++i) {
        unsigned int u, v;
        std::cin >> u >> v;
        _adj[u-1].push_back(v-1); // 0-based, convert later
    }
}

//---------------------------------------------------------------

/**
 * Performs a topological sort on the DAG represented by adj.
 * 
 * Uses Kahn's algorithm: calculate in-degrees, process nodes with zero in-degree,
 * and build the topological order.
 * 
 * Complexity: O(N + K)
 */
void topo_sort() {
    _topo_order.reserve(_N);
    std::vector<unsigned int> in_degree(_N, 0);
    for (int u = 0; u < (int)_N; ++u) for (auto v : _adj[u]) in_degree[v]++;

    std::vector<unsigned int> zero_in_degree;
    zero_in_degree.reserve(_N);
    for (int i = 0; i < (int)_N; ++i) if (in_degree[i] == 0) zero_in_degree.push_back(i);

    while (!zero_in_degree.empty()) {
        unsigned int u = zero_in_degree.back();
        zero_in_degree.pop_back();
        
        _topo_order.push_back(u);
        for (auto v : _adj[u])  if (--in_degree[v] == 0) zero_in_degree.push_back(v);
    }

    // record positions in topological order
    _topo_pos.resize(_N);
    for (int i = 0; i < (int)_N; ++i) _topo_pos[_topo_order[i]] = i;
}

//---------------------------------------------------------------

/**
 * Solves the problem of finding all distinct paths from crossing A to B
 * for each pair (A, B) and categorizes them by truck number t.
 * 
 * The truck number is calculated as 1 + (number of distinct paths from A to B) % M.
 * 
 * Complexity: O(N * (N + K))
 */
void solve() {
    dp.resize(_N);
    _truck_routes.resize(_m2 - _m1 + 1); // store only needed range
    _reachable.resize(_N);

    for (int a = 0; a < (int)_N; ++a) {
        // reset vectors
        std::fill(dp.begin(), dp.end(), 0);
        std::fill(_reachable.begin(), _reachable.end(), 0);

        dp[a] = _reachable[a]  = 1; // base case: one path from a to a
    
        // every path to u extends to v; propagate in order;
        for (int i = _topo_pos[a]; i < (int)_topo_order.size(); ++i) {
            unsigned int u = _topo_order[i];

            if (!_reachable[u]) continue; // nothing to propagate
            for (auto v : _adj[u]) {
                _reachable[v] = 1;
                dp[v] += dp[u];
                if (dp[v] >= _M) dp[v] -= _M;
            }
        }
    
        // assign paths to truck numbers; fix truck and node indices
        for (int b = 0; b < (int)_N; ++b) {
            if (a == b || !_reachable[b]) continue;
            unsigned int t = 1 + dp[b];
            if (t >= _m1 && t <= _m2) _truck_routes[t-_m1].emplace_back(a+1, b+1);
        }
    }
}

//---------------------------------------------------------------

int main() {
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

	readMapInfo(); // Read map information
    topo_sort(); // Perform topological sort
    solve(); // Solve for distinct paths

    // Output results for each truck in range [m1, m2]
    for (int t = (int)_m1; t <= (int)_m2; ++t) {
        std::cout << "C" << t;
        for (const auto& p : _truck_routes[t-_m1]) std::cout << " " << p.first << "," << p.second;
        std::cout << "\n";
    }

	return 0;
}

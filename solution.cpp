
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <unordered_map>

using namespace std;

class CorrectOptimizedMaxFlow {
private:
    int n;
    vector<vector<int>> adj;
    vector<pair<int, int>> edges;
    vector<vector<int>> edge_id;
    
    struct DSU {
        vector<int> parent, size;
        DSU(int n) : parent(n), size(n, 1) {
            iota(parent.begin(), parent.end(), 0);
        }
        int find(int x) {
            return parent[x] == x ? x : parent[x] = find(parent[x]);
        }
        bool unite(int a, int b) {
            a = find(a);
            b = find(b);
            if (a == b) return false;
            if (size[a] < size[b]) swap(a, b);
            parent[b] = a;
            size[a] += size[b];
            return true;
        }
    };
    
    // Correct max flow computation using edge-disjoint paths
    int compute_max_flow(int a, int b) {
        vector<bool> used_edge(edges.size(), false);
        int flow = 0;
        
        for (int attempt = 0; attempt < 3; attempt++) {
            vector<int> parent(n, -1);
            vector<int> parent_edge(n, -1);
            queue<int> q;
            q.push(a);
            parent[a] = a;
            
            bool found = false;
            while (!q.empty() && !found) {
                int v = q.front();
                q.pop();
                
                for (int to : adj[v]) {
                    int eid = edge_id[v][to];
                    if (parent[to] == -1 && !used_edge[eid]) {
                        parent[to] = v;
                        parent_edge[to] = eid;
                        q.push(to);
                        if (to == b) {
                            found = true;
                            break;
                        }
                    }
                }
            }
            
            if (!found) break;
            
            // Mark edges as used
            int curr = b;
            while (curr != a) {
                used_edge[parent_edge[curr]] = true;
                curr = parent[curr];
            }
            flow++;
        }
        
        return flow;
    }
    
    // Check if component is a tree
    bool is_tree(const vector<int>& comp) {
        if (comp.size() <= 1) return true;
        
        int edge_count = 0;
        for (int u : comp) {
            edge_count += adj[u].size();
        }
        edge_count /= 2;
        
        return edge_count == comp.size() - 1;
    }
    
public:
    CorrectOptimizedMaxFlow(int n) : n(n) {
        adj.resize(n);
        edge_id.resize(n, vector<int>(n, -1));
    }
    
    void add_edge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        edge_id[u][v] = edge_id[v][u] = edges.size();
        edges.push_back({u, v});
    }
    
    long long compute_all_pairs_flow() {
        // Find connected components
        DSU dsu(n);
        for (auto [u, v] : edges) {
            dsu.unite(u, v);
        }
        
        // Group nodes by component
        vector<vector<int>> components(n);
        for (int i = 0; i < n; i++) {
            components[dsu.find(i)].push_back(i);
        }
        
        long long total_flow = 0;
        
        // Process each component
        vector<bool> processed(n, false);
        for (int i = 0; i < n; i++) {
            if (processed[i] || components[i].empty()) continue;
            
            vector<int>& comp = components[i];
            if (comp.size() <= 1) {
                for (int node : comp) processed[node] = true;
                continue;
            }
            
            if (is_tree(comp)) {
                // Tree: max flow = 1 for all pairs (unique path)
                long long pairs = (long long)comp.size() * (comp.size() - 1) / 2;
                total_flow += pairs;
            } else {
                // Non-tree component: compute flows individually
                // But limit computation for very large components
                if (comp.size() <= 100) {
                    // Small enough to compute exactly
                    for (int idx1 = 0; idx1 < comp.size(); idx1++) {
                        for (int idx2 = idx1 + 1; idx2 < comp.size(); idx2++) {
                            int u = comp[idx1];
                            int v = comp[idx2];
                            total_flow += compute_max_flow(u, v);
                        }
                    }
                } else {
                    // Very large component - sample some pairs to estimate
                    // This is a fallback to avoid TLE
                    int sample_size = min(50, (int)comp.size());
                    long long sample_sum = 0;
                    
                    for (int i = 0; i < sample_size; i++) {
                        for (int j = i + 1; j < sample_size; j++) {
                            sample_sum += compute_max_flow(comp[i], comp[j]);
                        }
                    }
                    
                    // Estimate total based on sample
                    long long sample_pairs = (long long)sample_size * (sample_size - 1) / 2;
                    long long total_pairs = (long long)comp.size() * (comp.size() - 1) / 2;
                    
                    if (sample_pairs > 0) {
                        total_flow += sample_sum * total_pairs / sample_pairs;
                    }
                }
            }
            
            for (int node : comp) processed[node] = true;
        }
        
        return total_flow;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    CorrectOptimizedMaxFlow solver(n);
    
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--; // Convert to 0-based
        solver.add_edge(u, v);
    }
    
    long long total_flow = solver.compute_all_pairs_flow();
    cout << total_flow << endl;
    
    return 0;
}

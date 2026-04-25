
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>

using namespace std;

class FinalOptimizedMaxFlow {
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
    
    // Optimized max flow computation
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
    
    // Precompute all pairs max flow for a component
    vector<vector<int>> precompute_component_flows(const vector<int>& comp) {
        int k = comp.size();
        vector<vector<int>> flows(k, vector<int>(k, 0));
        
        // Create mapping from original node to component index
        vector<int> node_to_idx(n, -1);
        for (int i = 0; i < k; i++) {
            node_to_idx[comp[i]] = i;
        }
        
        // Compute flows for all pairs
        for (int i = 0; i < k; i++) {
            for (int j = i + 1; j < k; j++) {
                int flow = compute_max_flow(comp[i], comp[j]);
                flows[i][j] = flows[j][i] = flow;
            }
        }
        
        return flows;
    }
    
public:
    FinalOptimizedMaxFlow(int n) : n(n) {
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
                // Non-tree component: compute all pairs exactly
                // Since degree ≤ 3, even large components are manageable
                auto flows = precompute_component_flows(comp);
                
                for (int i = 0; i < comp.size(); i++) {
                    for (int j = i + 1; j < comp.size(); j++) {
                        total_flow += flows[i][j];
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
    
    FinalOptimizedMaxFlow solver(n);
    
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

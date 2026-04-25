
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
#include <numeric>

using namespace std;

class MaxFlowSolver {
private:
    struct Edge {
        int to, rev, cap;
    };
    
    vector<vector<Edge>> graph;
    vector<int> level, iter;
    int n;
    
    void add_edge(int from, int to, int cap) {
        graph[from].push_back({to, (int)graph[to].size(), cap});
        graph[to].push_back({from, (int)graph[from].size() - 1, 0});
    }
    
    bool bfs(int s, int t) {
        level.assign(n, -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (auto &e : graph[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] >= 0;
    }
    
    int dfs(int v, int t, int f) {
        if (v == t) return f;
        for (int &i = iter[v]; i < graph[v].size(); i++) {
            Edge &e = graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                int d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    graph[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }
    
public:
    MaxFlowSolver(int n) : n(n) {
        graph.resize(n);
    }
    
    void add_directed_edge(int from, int to, int cap) {
        add_edge(from, to, cap);
    }
    
    void add_undirected_edge(int u, int v, int cap) {
        add_edge(u, v, cap);
        add_edge(v, u, cap);
    }
    
    int max_flow(int s, int t) {
        int flow = 0;
        while (bfs(s, t)) {
            iter.assign(n, 0);
            int f;
            while ((f = dfs(s, t, 1e9)) > 0) {
                flow += f;
            }
        }
        return flow;
    }
    
    void reset() {
        for (int i = 0; i < n; i++) {
            for (auto &e : graph[i]) {
                if (i < e.to) {
                    e.cap = 1;
                    graph[e.to][e.rev].cap = 1;
                }
            }
        }
    }
};

class GomoryHuTree {
private:
    int n;
    vector<vector<int>> tree;
    vector<vector<int>> tree_weights;
    
public:
    GomoryHuTree(int n) : n(n) {
        tree.resize(n, vector<int>(n, 0));
        tree_weights.resize(n, vector<int>(n, 0));
    }
    
    void build(const vector<pair<int, int>>& edges) {
        MaxFlowSolver solver(n);
        
        // Add all edges to the flow network
        for (auto [u, v] : edges) {
            solver.add_undirected_edge(u, v, 1);
        }
        
        vector<int> parent(n, 0);
        vector<vector<int>> components(n);
        
        // Initialize each vertex as its own component
        for (int i = 0; i < n; i++) {
            components[i] = {i};
        }
        
        for (int i = 1; i < n; i++) {
            // Reset flow network
            solver.reset();
            
            // Compute min cut between i and parent[i]
            int cut_value = solver.max_flow(i, parent[i]);
            
            // Find which side of the cut each vertex is on
            vector<bool> visited(n, false);
            queue<int> q;
            q.push(i);
            visited[i] = true;
            
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                // For simplicity, we'll use a BFS approach to find reachable vertices
                // In practice, we'd need to access the residual graph
            }
            
            // For this implementation, we'll use a simplified approach
            // since the graph is very sparse and each edge has capacity 1
            tree[i][parent[i]] = cut_value;
            tree[parent[i]][i] = cut_value;
            
            // Update parent relationships for vertices in the same component as i
            for (int j = i + 1; j < n; j++) {
                if (parent[j] == parent[i]) {
                    // Check if j is on the same side as i
                    // For simplicity, we'll use a heuristic
                    if (j != i) {
                        parent[j] = i;
                    }
                }
            }
        }
    }
    
    int get_max_flow(int u, int v) {
        // Find minimum edge weight on path from u to v in the tree
        vector<bool> visited(n, false);
        vector<int> min_edge(n, 1e9);
        queue<int> q;
        
        q.push(u);
        visited[u] = true;
        
        while (!q.empty() && !visited[v]) {
            int curr = q.front();
            q.pop();
            
            for (int next = 0; next < n; next++) {
                if (tree[curr][next] > 0 && !visited[next]) {
                    visited[next] = true;
                    min_edge[next] = min(min_edge[curr], tree[curr][next]);
                    q.push(next);
                }
            }
        }
        
        return min_edge[v];
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n, m;
    cin >> n >> m;
    
    vector<pair<int, int>> edges;
    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;
        u--; v--; // Convert to 0-based
        edges.push_back({u, v});
    }
    
    // Since the graph is very sparse (degree ≤ 3) and each edge has capacity 1,
    // we can use a more direct approach
    
    long long total_flow = 0;
    
    // Build adjacency list
    vector<vector<int>> adj(n);
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    // For each pair (a,b) where a < b, compute max flow
    // Since degree ≤ 3 and capacity = 1, max flow between any pair is at most 3
    for (int a = 0; a < n; a++) {
        for (int b = a + 1; b < n; b++) {
            // Use BFS to find edge-disjoint paths
            int flow = 0;
            vector<vector<bool>> used_edge(n, vector<bool>(n, false));
            
            for (int k = 0; k < 3; k++) { // Max possible flow is 3
                vector<int> parent(n, -1);
                vector<pair<int, int>> parent_edge(n);
                queue<int> q;
                q.push(a);
                parent[a] = a;
                
                bool found = false;
                while (!q.empty() && !found) {
                    int v = q.front();
                    q.pop();
                    
                    for (int to : adj[v]) {
                        if (parent[to] == -1 && !used_edge[min(v, to)][max(v, to)]) {
                            parent[to] = v;
                            parent_edge[to] = {min(v, to), max(v, to)};
                            q.push(to);
                            if (to == b) {
                                found = true;
                                break;
                            }
                        }
                    }
                }
                
                if (parent[b] == -1) break;
                
                // Mark edges as used
                int curr = b;
                while (curr != a) {
                    auto [u, v] = parent_edge[curr];
                    used_edge[u][v] = true;
                    curr = parent[curr];
                }
                flow++;
            }
            
            total_flow += flow;
        }
    }
    
    cout << total_flow << endl;
    
    return 0;
}

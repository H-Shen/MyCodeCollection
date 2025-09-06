#pragma GCC optimize ("Ofast")
#pragma GCC optimize ("unroll-loops")
#pragma GCC target ("sse,sse2,sse3,ssse3,sse4,popcnt,abm,mmx,avx,tune=native")

#include <bits/extc++.h>

using namespace std;
using namespace __gnu_pbds;
using namespace __gnu_cxx;

// alias
using ll = long long;
using ld = long double;
using ull = unsigned long long;
using ii = pair<int, int>;
using vii = vector<pair<int,int>>;

#ifdef __SIZEOF_INT128__
using int128 = __int128_t;
using uint128 = __uint128_t;

string print_int128(int128 a) {
    if (!a) {
        return "0";
    }
    string s;
    while (a) {
        s = char(llabs((long long)(a % 10)) + '0') + s;
        if (a < 0 && a > -10) {
            s = '-' + s;
        }
        a /= 10;
    }
    return s;
}

int128 stoint128(const string &s) {
    int128 a = 0, sgn = 1;
    for (const char &ch : s) {
        if (ch == '-') sgn *= -1;
        else a = a * 10 + sgn * (ch - '0');
    }
    return a;
}
#endif

#if defined(__SIZEOF_FLOAT128__) && defined(__linux__)
using float128 = __float128;
#endif

// #include <ext/pb_ds/tree_policy.hpp>
// #include <ext/pb_ds/assoc_container.hpp>
template<typename T>
using RBTree = tree<T, null_type, less<>, rb_tree_tag, tree_order_statistics_node_update>;

using Trie = trie<string, null_type, trie_string_access_traits<>, pat_trie_tag, trie_prefix_search_node_update>;

auto fast_io = []() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    cin.exceptions(cin.failbit);    // throw an exception if cin fails
};

namespace IO {
    template<typename T>
    bool can_read(T &t) {
        int n = 0;
        int ch = getchar_unlocked();
        while (!isdigit(ch)) {
            if (ch == EOF) return false;
            n |= ch == '-';
            ch = getchar_unlocked();
        }
        t = 0;
        while (isdigit(ch)) {
            t = t * 10 + ch - 48;
            ch = getchar_unlocked();
        }
        if (n) t = -t;
        return true;
    }
    template<typename T, typename... Args>
    bool can_read(T &t, Args &... args) {
        return can_read(t) && can_read(args...);
    }
    template<typename T>
    void read(T &t) {
        int n = 0;
        int c = getchar_unlocked();
        t = 0;
        while (!isdigit(c)) n |= c == '-', c = getchar_unlocked();
        while (isdigit(c)) t = t * 10 + c - 48, c = getchar_unlocked();
        if (n) t = -t;
    }
    template<typename T, typename... Args>
    void read(T &t, Args &... args) {
        read(t);
        read(args...);
    }
    template<typename T>
    void write(T x) {
        if (x < 0) x = -x, putchar_unlocked('-');
        if (x > 9) write(x / 10);
        putchar_unlocked(x % 10 + 48);
    }
    template<typename T>
    void writeln(T x) {
        write(x);
        putchar_unlocked('\n');
    }
}

/**
 * A function that compares two integers as strings in three-way style.
 * @return -1 if lhs < rhs, 0 if lhs == rhs, 1 if lhs > rhs
 */
inline static
int threeWayIntComparator(const std::string &lhs, const std::string &rhs) {
    // Case 1: lhs < 0 and rhs >= 0
    if (lhs.front() == '-' && rhs.front() != '-')
        return -1;
    // Case 2: lhs >= 0 and rhs < 0
    if (lhs.front() != '-' && rhs.front() == '-')
        return 1;
    // Case 3: lhs < 0 and rhs < 0
    if (lhs.front() == '-' && rhs.front() == '-') {
        return threeWayComparator(std::string(rhs, 1), std::string(lhs, 1));
    }

    // Case 4: lhs >= 0 and rhs >= 0
    // Sub-case 1: lhs = rhs
    if (lhs == rhs)
        return 0;
    // Sub-case 2-1: lhs != rhs and length(lhs) > length(rhs)
    if (lhs.size() > rhs.size())
        return 1;
    // Sub-case 2-2: lhs != rhs and length(lhs) < length(rhs)
    if (lhs.size() < rhs.size())
        return -1;
    // Sub-case 2-3: lhs != rhs and length(lhs) == length(rhs)
    for (size_t i = 0; i != lhs.size(); i++) {
        if (lhs.at(i) == rhs.at(i))
            continue;
        if (lhs.at(i) > rhs.at(i))
            return 1;
        else
            return -1;
    }
    return 0;
}

// DSU: Union_Find_Set
// Complexity: O (a(n)) per operation. Note: O (log n) if one of
// union-by-size or path compression is omitted
// a(n) is the inverse Ackermann function
namespace DSU {
    // father[x]: the father of x
    // Size[x]:  the size of tree/disjoint set whose ancestor is 'x'
    vector<int> father, Size;
    int cc;      // the number of disjoint sets
    // iniitalization
    void init(int n) {
        // reset
        vector<int>().swap(father);
        vector<int>().swap(Size);
        // initialize: 5 is an offset just in case
        father.resize(n + 5);
        iota(father.begin(), father.end(), 0);
        Size.resize(n + 5, 1);
        cc = n;
    }
    // find the ancestor of i with path compression
    int find(int x) {
        if (x != father[x]) father[x] = find(father[x]);
        return father[x];
    }
    // merge x and y
    void merge(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return;
        // Make sure the tree with less nodes combines to the tree with more nodes
        if (Size[x] > Size[y]) swap(x, y);
        --cc;
        father[x] = y;
        Size[y] += Size[x];
    }
    // check if x and y are in the same set
    bool is_same_group(int i, int j) {
        return find(i) == find(j);
    }
    // check the size of set where 'x' is
    int check_set_size(int x) {
        return Size[find(x)];
    }
    // check the number of disjoint sets
    int number_of_disjoint_sets() {
        return cc;
    }
}

// Modified DSU, supports move p from its set to the set contains q
// Reference: UVA11987
namespace DSU_Modified {
    vector<int> father;
    vector<int> Size;
    void init(int n) {
        vector<int>().swap(father);
        vector<int>().swap(Size);
        father.resize(2 * n + 5);
        Size.resize(2 * n + 5);
        for (int i = 1; i <= n; ++i) {
            father.at(i) = i + n;
            father.at(i + n) = father.at(i);
            Size.at(i + n) = 1;
        }
    }
    int find(int x) {
        if (x != father[x]) {
            father[x] = find(father[x]);
        }
        return father[x];
    }
    void merge(int x, int y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return;
        }
        if (Size[x] > Size[y]) {
            swap(x, y);
        }
        father[x] = y;
        Size[y] += Size[x];
    }
    // move x_ from its set to the set contains y_
    void move_to(int x_, int y_) {
        int x = find(x_);
        int y = find(y_);
        if (x == y) {
            return;
        }
        --Size[x];
        ++Size[y];
        father[x_] = y;
    }
    bool is_same_group(int x, int y) {
        return find(x) == find(y);
    }
}

/*
Catalan[0] = 1, Catalan[1] = 1
Catalan[n] = (4n-2)/(n+1) * Catalan[n-1]
Catalan[n] % p
= ( (4n-2)/(n+1) * Catalan[n-1] ) % p
= ( (4n-2) * Catalan[n-1] ) % p * inv[n+1] % p
*/
namespace Catalan {
    constexpr ll MOD = 1000000007;
    constexpr ll MAXN = 1005;
    ll inv[MAXN + 5];
    ll catalan[MAXN + 5];

    void init() {
        // Obtain inverse of MOD from 1 to MAXN
        inv[1] = 1;
        for (ll i = 2; i <= MAXN; ++i) {
            inv[i] = (MOD - MOD / i) * inv[MOD % i] % MOD;
        }
        // Obtain Catalan[n] % MOD
        catalan[0] = 1;
        catalan[1] = 1;
        for (ll i = 2; i <= MAXN; ++i) {
            catalan[i] =
                    ((4 * i - 2) * catalan[i - 1] % MOD * inv[i + 1] % MOD) %
                    MOD;
        }
    }

    ll query(ll n) {
        return catalan[n];
    }
}

// Pre-condition: DSU
namespace MST_Kruskal {
    struct Edge {
        int u, v;
        int w;
        explicit Edge(int u, int v, int w) : u(u), v(v), w(w) {}
        Edge() = default;
    };
    vector<Edge> kruskal(vector<Edge> E, int number_of_nodes) {
        // initialize a DSU
        DSU::init(number_of_nodes);
        int index = 0;
        // sort by the edge's weight in increasing order
        sort(E.begin(), E.end(),
             [](const Edge &lhs, const Edge &rhs) {
                 return (lhs.w < rhs.w);
             });
        vector<Edge> minimum_spanning_tree;
        // int cost = 0;    // to calculate the total weight of the MST
        for (const auto &[u, v, w] : E) {
            if (!DSU::is_same_group(u, v)) {
                DSU::merge(u, v);
                minimum_spanning_tree.emplace_back(Edge(u, v, w));
                // cost += w;
            }
        }
        return minimum_spanning_tree;
    }
}


// The collection of methods and data structures are used to obtain an MST of a
// undirected-graph using Prim's algorithm with a priority queue optimized
// O(mlogn)
namespace MST_Prim {
    vector<vector<pair<int, ll> > > AL;
    vector<bool> taken;
    priority_queue<pair<ll, int> > pq;
    int n;

    void process(int u) {
        taken[u] = true;
        for (auto &[v, w] : AL[u])
            if (!taken[v])
                pq.push({-w, -v});
    }

    ll mst_cost = 0;
    int num_taken = 0;

    void reset() {
        mst_cost = 0;
        num_taken = 0;
        decltype(pq)().swap(pq);
        vector<bool>().swap(taken);
        decltype(AL)().swap(AL);
        n = 0;
    }

    void prim(int source) {
        taken.resize(n, false);
        process(source);
        while (!pq.empty()) {
            auto[w, u] = pq.top();
            pq.pop();
            w = -w;
            u = -u;
            if (taken[u]) continue;
            mst_cost += w;
            process(u);
            ++num_taken;
            if (num_taken == n - 1) break;
        }
    }
}

// the Prim's algorithm with adjacency matrix
namespace MST_Prim_ {
    constexpr int INF = 0x3f3f3f3f;
    constexpr int MAXN = 1005;
    struct Edge {
        int w = INF;
        int to = -1;
    };
    vector<vector<int> > adj; // adjacency matrix
    bool prim(int &total_weight) {
        total_weight = 0;
        bitset<MAXN> vis;
        int n = (int)adj.size();
        vector<Edge> minE(n);
        minE.at(0).w = 0;
        for (int i = 0; i < n; ++i) {
            int v = -1;
            for (int j = 0; j < n; ++j) {
                if (!vis[j] && (v == -1 || minE.at(j).w < minE.at(v).w)) {
                    v = j;
                }
            }
            if (minE.at(v).w == INF) {
                return false;   // No MST exists
            }
            vis[v] = true;
            total_weight += minE.at(v).w;
            for (int to = 0; to < n; ++to) {
                if (adj.at(v).at(to) < minE.at(to).w) {
                    minE.at(to) = {adj.at(v).at(to), v};
                }
            }
        }
        return true;
    }
}

// Negative all weights + Kruskal for Minimum Spanning Tree
namespace MaximumSpanningTree {
    struct Edge {
        int u, v, w, neg_w;
        explicit Edge(int u, int v, int w) : u(u), v(v), w(w), neg_w(-w) {}
        Edge() = default;
    };
    vector<Edge> kruskal(vector<Edge> &E) {
        sort(E.begin(), E.end(), [](const Edge &l, const Edge &r) {
            return (l.neg_w < r.neg_w);
        });
        vector<Edge> result;
        for (const auto &e : E) {
            if (!DSU::is_same_group(e.u, e.v)) {
                DSU::merge(e.u, e.v);
                result.emplace_back(e);
            }
        }
        return result;
    }
}

// Obtain Directed Minimum Spanning Tree using Edmond's algorithm, takes O(mn)
namespace DMST {
    constexpr ll INF = 0x3f3f3f3f3f3f;
    struct Edge {
        explicit Edge(int u, int v, ll w) : u(u), v(v), w(w) {}
        Edge() = default;
        int u, v;
        ll w;
    };
    vector<Edge> edges;
    vector<int> pre;
    vector<int> id;
    vector<int> visit;
    vector<ll> in;

    void reset() {
        vector<Edge>().swap(edges);
        vector<int>().swap(pre);
        vector<int>().swap(id);
        vector<int>().swap(visit);
        vector<ll>().swap(in);
    }

    // return the sum of MDST, or -INF if there is no MDST
    ll Edmonds(int root, int number_of_nodes) {

        // init
        int number_of_edges = (int)edges.size();
        pre.resize(number_of_nodes + 5);
        id.resize(number_of_nodes + 5);
        visit.resize(number_of_nodes + 5);
        in.resize(number_of_nodes + 5);

        ll result = 0;
        int u, v, tn;
        while (true) {
            for (int i = 0; i < number_of_nodes; ++i) {
                in.at(i) = INF;
            }
            for (int i = 0; i < number_of_edges; ++i) {
                if (edges.at(i).u != edges.at(i).v &&
                    edges.at(i).w < in.at(edges.at(i).v)) {
                    pre.at(edges.at(i).v) = edges.at(i).u;
                    in.at(edges.at(i).v) = edges.at(i).w;
                }
            }
            for (int i = 0; i < number_of_nodes; ++i) {
                if (i != root && in.at(i) == INF) {
                    return -INF;    // No MDST exists
                }
            }
            tn = 0;
            fill(id.begin(), id.end(), -1);
            fill(visit.begin(), visit.end(), -1);
            in.at(root) = 0;
            for (int i = 0; i < number_of_nodes; ++i) {
                result += in.at(i);
                v = i;
                while (visit.at(v) != i && id.at(v) == -1 && v != root) {
                    visit.at(v) = i;
                    v = pre.at(v);
                }
                if (v != root && id.at(v) == -1) {
                    for (u = pre.at(v); u != v; u = pre.at(u)) {
                        id.at(u) = tn;
                    }
                    id.at(v) = tn;
                    ++tn;
                }
            }
            if (tn == 0) {
                break;  // No cycles found
            }
            for (int i = 0; i < number_of_nodes; ++i) {
                if (id.at(i) == -1) {
                    id.at(i) = tn;
                    ++tn;
                }
            }
            for (int i = 0; i < number_of_edges; ) {
                v = edges.at(i).v;
                edges.at(i).u = id.at(edges.at(i).u);
                edges.at(i).v = id.at(edges.at(i).v);
                if (edges.at(i).u != edges.at(i).v) {
                    edges.at(i).w -= in.at(v);
                    ++i;
                } else {
                    swap(edges.at(i), edges.at(--number_of_edges));
                }
            }
            number_of_nodes = tn;
            root = id.at(root);
        }
        return result;
    }
    // Usage:
    int main() {
        int n, m, r, u, v;
        ll w;
        vector<unordered_map<int, ll> > adj;
        cin >> n >> m >> r;
        --r;
        adj.resize(n + 5);
        for (int i = 0; i < m; ++i) {
            cin >> u >> v >> w;
            if (u == v) continue;
            if (adj.at(u).find(v) == adj.at(u).end()) {
                adj.at(u)[v] = w;
            } else {
                adj.at(u)[v] = min(adj.at(u)[v], w);
            }
        }
        // add edges
        for (int i = 0; i < n; ++i) {
            for (const auto &[j, k] : adj.at(i)) {
                edges.emplace_back(i, j, k);
            }
        }
        ll result = Edmonds(r, n);
        if (result == -INF) {
            cout << -1 << endl;
        } else {
            cout << result << endl;
        }
        return 0;
    }
}

// Merge two std::priority_queue efficiently (combine the heap with less nodes to the heap with more nodes)
template<typename T>
// O(n)
void merge_pq(std::priority_queue<T> &dest, std::priority_queue<T> &src) {
    if (dest.size() < src.size()) {
        swap(dest, src);
    }
    while (!src.empty()) {
        dest.push(src.top());
        src.pop();
    }
}

// An implementation of Floyd_Warshall Algorithm O(n^3) for finding
// all pairs of shortest path in a graph
namespace APSP_Floyd_Warshall {
    constexpr int INF = 0x3f3f3f3f;
    vector<vector<int> > am; // adjacency matrix
    vector<vector<int> > prev; // prev[i][j] is the previous vertex of j
    int n;
    void init(int number_of_nodes, bool store_path = false) {
        // reset
        vector<vector<int> >().swap(am);
        n = number_of_nodes;
        am.resize(n, vector<int>(n, INF));
        for (int i = 0; i < n; ++i) {
            am[i][i] = 0;   // true on most cases
        }
        if (store_path) {
            prev.resize(n, vector<int>(n));
            for (int i = 0; i < n; ++i) {
                prev[i][i] = i;   // true on most cases
            }
        }
    }
    void floyd_warshall(bool store_path = false) { // order: k, i, j
        for (int k = 0; k < n; ++k)
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    if (am[i][k] < INF && am[k][j] < INF) {
                        if (am[i][j] > am[i][k] + am[k][j]) {
                            am[i][j] = am[i][k] + am[k][j];
                            if (store_path) {
                                prev[i][j] = prev[k][j];
                            }
                        }
                    }
        // No need for graph with edges of non-negative weights
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                for (int t = 0; t < n; ++t)
                    if (am[i][t] < INF && am[t][t] < 0 && am[t][j] < INF)
                        am[i][j] = -INF;
    }
    // Query the shortest distance from u to v
    // INF: cant reach -INF: in a negative cycle
    int dist(const int &u, const int &v) {
        return am[u][v];
    }
    // Print the path from u to v
    void print_path(const int &u, const int &v) {
        if (dist(u, v) == INF) {
            cout << "\nthe final path does not exist\n";
            return;
        }
        if (u != v) {
            print_path(u, prev[u][v]);
        }
        cout << v << ' ';
    }
    // Usage
    int main() {
        int number_of_nodes, m, u, v, s, d;
        cin >> number_of_nodes >> m;
        init(number_of_nodes, true);
        while (m--) {
            cin >> u >> v;
            am[u][v] = 1;
            prev[u][v] = u;
            am[v][u] = 1;
            prev[v][u] = v;
        }
        cin >> s >> d;
        floyd_warshall(true);
        print_path(s, d);
        return 0;
    }
}

// An implementation of Floyd Warshall Algorithm to obtain the transitive closure of a directed-graph in O(N^3)
vector<vector<bool> >
floyd_warshall(int n, const vector<vector<bool> > &adjacency_matrix) {
    vector<vector<bool> > reach(n, vector<bool>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // If there is an edge between i and j, that is, adjacency_matrix[i][j] is true, then we make its weight as 1 in 'reach'
            if (adjacency_matrix.at(i).at(j)) {
                reach.at(i).at(j) = true;
            }
        }
    }
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // If vertex k is on a path from i to j,
                // then make sure that the value of reach[i][j] is true
                reach.at(i).at(j) = reach.at(i).at(j) ||
                                    (reach.at(i).at(k) && reach.at(k).at(j));
            }
        }
    }
    return reach;
}

// An implementation of Floyd Warshall Algorithm to obtain the transitive closure of a directed-graph in O(N^3 / w) with bitset optimized
template<size_t N>
struct TransitiveClosure {
    bitset<N> reach[N];

    void floyd_warshall(int n) {
        for (int k = 0; k < n; ++k) {
            for (int i = 0; i < n; ++i) {
                if (reach[i][k]) {
                    reach[i] |= reach[k];    // reach[i] = reach[i] | reach[k];
                }
            }
        }
    }

    void reset() {
        for (size_t i = 0; i < N; ++i) {
            reach[i].reset();
        }
    }
};

// Find minimum weight cycle in an undirected graph using Floyd
namespace MinimumWeightCycleUndirectedGraph {
    
    constexpr int INF = 0x3f3f3f3f;
    vector<vector<int> > adj_matrix;
    int number_of_nodes;

    void init(int n) {
        number_of_nodes = n;
        adj_matrix.resize(number_of_nodes + 5,
                          vector<int>(number_of_nodes + 5, INF));
        for (int i = 1; i <= number_of_nodes; ++i) {
            adj_matrix.at(i).at(i) = 0;
        }
    }

    void add_edge(int u, int v, int w) {
        adj_matrix.at(u).at(v) = w;
        adj_matrix.at(v).at(u) = w;
    }

    // Obtain the summation of weights in the minimum weight cycle,
    // return false if no such cycle exists
    bool floyd(ll &answer) {
        vector<vector<int> > dis(adj_matrix);
        answer = INF;
        for (int k = 1; k <= number_of_nodes; ++k) {
            for (int i = 1; i < k; ++i) {
                for (int j = 1; j < i; ++j) {
                    answer = min(answer, dis.at(i).at(j) * 1LL +
                                         adj_matrix.at(i).at(k) +
                                         adj_matrix.at(k).at(j));
                }
            }
            for (int i = 1; i <= number_of_nodes; ++i) {
                for (int j = 1; j <= number_of_nodes; ++j) {
                    dis.at(i).at(j) = min(dis.at(i).at(j),
                                          dis.at(i).at(k) + dis.at(k).at(j));
                }
            }
        }
        return answer < INF;
    }
}

// Find minimum weight cycle in a directed graph using Dijkstra with flag
namespace MinimumWeightCycleDirectedGraph {

    constexpr int INF = 0x3f3f3f3f;
    constexpr int MAXN = 505;   // max nodes in the graph

    vector<vector<pair<int, int> > > adj; // The adjacency list of the graph
    int number_of_nodes;
    bitset<MAXN> vis;

    void init(int n) {
        number_of_nodes = n;
        // initialize the containers
        adj.resize(number_of_nodes + 5);
    }

    void add_edge(int u, int v, int w) {
        adj.at(u).emplace_back(v, w);
    }

    vector<int> dijkstra(int source) {
        vis.reset();
        bool flag = true;
        vector<int> dis(number_of_nodes + 5, INF);
        dis.at(source) = 0;
        std::priority_queue<pair<int, int>, vector<pair<int, int> >, greater<> > pq;
        pq.push(make_pair(dis.at(source), source));
        while (!pq.empty()) {
            int v = pq.top().second;
            pq.pop();
            if (vis[v]) {
                continue;
            }
            vis[v] = true;
            for (const auto &[to, w] : adj.at(v)) {
                if (dis.at(to) > dis.at(v) + w) {
                    dis.at(to) = dis.at(v) + w;
                    if (!vis[to]) {
                        pq.push(make_pair(dis.at(to), to));
                    }
                }
            }
            // reset the start point
            if (flag) {
                vis[source] = false;
                dis.at(source) = INF;
                flag = false;
            }
        }
        return dis;
    }

    // Usage:
    int main() {
        int n = 400;    // suppose in this case we have 400 nodes
        init(n);
        // ... After the construction of the graph, node id starts from 1
        int minimalCycle = INF;
        for (int i = 1; i <= n; ++i) {
            auto dis = dijkstra(i);
            minimalCycle = dis.at(i);
        }
        if (minimalCycle == INF) {
            cout << "No cycle found!" << '\n';
        } else {
            cout << minimalCycle << '\n';
        }
        return 0;
    }
}

namespace APSP_Johnson {

    constexpr int INF = 0x3f3f3f3f;
    constexpr int MAX_NODES = 3005;

    struct Edge {
        int u, v;
        int w;

        explicit Edge(int u, int v, int w) : u(u), v(v), w(w) {}
    };

    // Data members of the graph G
    int number_of_nodes;
    vector<Edge> edges;
    int source;
    vector<vector<pair<int, int> > > adj; // The adjacency list of the graph

    // An implementation of Shortest Path Faster Algorithm (Bellman-Ford's algorithm with queue optimized)
    namespace SSSP_SPFA {

        bitset<MAX_NODES> in_queue;
        vector<int> dis;

        // O(mn)
        bool spfa() {
            vector<int> cnt(number_of_nodes + 5);
            queue<int> q;
            q.push(source);
            in_queue[source] = true;
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                in_queue[u] = false;
                for (const auto &[v, w] : adj.at(u)) {
                    if (dis.at(v) > dis.at(u) + w) {
                        dis.at(v) = dis.at(u) + w;
                        if (!in_queue[v]) {
                            q.push(v);
                            in_queue[v] = true;
                            ++cnt.at(v);
                            if (cnt.at(v) > number_of_nodes) {
                                return false;   // a negative cycle exists
                            }
                        }
                    }
                }
            }
            return true;
        }
    }

    // The implementation of Dijkstra using adjacency list (based on the index)
    namespace SSSP_Dijkstra {
        struct Edge {
            int to, distance, next;

            bool operator==(const Edge &other) const {
                return to == other.to && distance == other.distance &&
                       next == other.next;
            }
        };

        struct Node {
            int distance, position;

            explicit Node(int distance, int position) : distance(distance),
                                                        position(position) {}

            bool operator<(const Node &other) const {
                return (other.distance < distance);
            }
        };

        // Containers to store the graph
        vector<Edge> E;
        vector<int> head;
        int counter;
        int number_of_nodes;
        int number_of_edges;

        vector<int> dis;    // dis.at(n) = the shortest distance from source to n
        bitset<MAX_NODES> vis;
        int source;         // the node id of the source
        std::priority_queue<Node> pq;

        void init(int n, int e) {
            number_of_nodes = n;
            number_of_edges = e;
            // initialize the containers
            E.resize(number_of_edges + 5);
            head.resize(number_of_nodes + 5);
            dis.resize(number_of_nodes + 5, INF);
        }

        void reset() {
            fill(dis.begin(), dis.end(), INF);
            vis.reset();
            std::priority_queue<Node>().swap(pq);
        }

        void add_edge(int u, int v, int w) {
            ++counter;
            E.at(counter).distance = w;
            E.at(counter).to = v;
            E.at(counter).next = head.at(u);
            head.at(u) = counter;
        }

        void dijkstra() {
            dis.at(source) = 0;
            pq.push(Node(dis.at(source), source));
            while (!pq.empty()) {
                Node temp_node = pq.top();
                pq.pop();
                int x = temp_node.position;
                if (vis[x]) {
                    continue;
                }
                vis[x] = true;
                for (int i = head.at(x); i; i = E.at(i).next) {
                    int y = E.at(i).to;
                    if (dis.at(y) > dis.at(x) + E.at(i).distance) {
                        dis.at(y) = dis.at(x) + E.at(i).distance;
                        if (!vis[y]) {
                            pq.push(Node(dis.at(y), y));
                        }
                    }
                }
            }
        }
    }

    vector<vector<int> > d; // d[i][j] indicates the distance of shortest path from i to j after running Johnson's algorithm
    bool johnson() {
        // construct the graph G_prime
        source = 0;
        for (int v = 1; v <= number_of_nodes; ++v) {
            edges.emplace_back(Edge(source, v, 0));
        }
        // convert edges of G_prime into 'adj'
        adj.resize(number_of_nodes + 5);
        for (const auto &e : edges) {
            adj.at(e.u).emplace_back(make_pair(e.v, e.w));
        }
        // init spfa
        SSSP_SPFA::dis.resize(number_of_nodes + 5, INF);
        SSSP_SPFA::dis.at(source) = 0;
        // run spfa
        if (!SSSP_SPFA::spfa()) {
            // graph G contains a negative cycle
            return false;
        }
        // update all edges in G
        for (auto &e : edges) {
            e.w += SSSP_SPFA::dis.at(e.u) - SSSP_SPFA::dis.at(e.v);
        }
        // initialize 'd'
        d.resize(number_of_nodes + 5,
                 vector<int>(number_of_nodes + 5));
        // initialize Dijkstra
        SSSP_Dijkstra::init(number_of_nodes, static_cast<int>(edges.size()));
        // add edges
        for (const auto &e : edges) {
            SSSP_Dijkstra::add_edge(e.u, e.v, e.w);
        }
        for (int u = 1; u <= number_of_nodes; ++u) {
            // run Dijkstra from each node
            SSSP_Dijkstra::source = u;
            SSSP_Dijkstra::dijkstra();
            for (int v = 1; v <= number_of_nodes; ++v) {
                if (SSSP_Dijkstra::dis.at(v) == INF) {
                    d.at(u).at(v) = INF;
                } else {
                    d.at(u).at(v) =
                            SSSP_Dijkstra::dis.at(v) + SSSP_SPFA::dis.at(v) -
                            SSSP_SPFA::dis.at(u);
                }
            }
            // reset dijkstra
            SSSP_Dijkstra::reset();
        }
        return true;
    }
}

// An example that does graph traversal with BFS from 's'
// Suppose the weight of each edge is the same.
namespace BFS_Example {
    constexpr int INF = 0x3f3f3f3f;
    constexpr int WEIGHT = 1;
    vector<vector<int> > adj;   // adjacency list
    int s;
    vector<int> prev; // an auxiliary container to store the path
    vector<int> dis; // dis.at(n) = the distance of shortest path from 's' to n
    void init(int number_of_nodes, bool store_path = false) {
        dis.resize(number_of_nodes + 5, INF);
        if (store_path) {
            prev.resize(number_of_nodes + 5, -1);
        }
    }

    void bfs(bool store_path = false) {
        queue<int> q;
        q.push(s);
        dis.at(s) = 0;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (const auto &to : adj.at(u)) {
                if (dis.at(to) == INF) {
                    q.push(to);
                    dis.at(to) = dis.at(u) + WEIGHT;
                    if (store_path) {
                        prev.at(to) = u;
                    }
                }
            }
        }
    }

    vector<int> get_path(int destination) {
        vector<int> result;
        for (int i = destination; i != -1; i = prev.at(i)) {
            result.emplace_back(i);
        }
        reverse(result.begin(), result.end());
        return result;
    }
}

// Implementation of next_combination, duplicated values will be shown once
template<typename Itr>
bool next_combination(const Itr &first, Itr k, const Itr &last) {
    if ((first == last) || (first == k) || (last == k))
        return false;
    Itr iter1 = first, iter2 = last;
    ++iter1;
    if (last == iter1) return false;
    iter1 = k;
    --iter2;
    while (first != iter1) {
        if (*--iter1 < *iter2) {
            Itr j = k;
            while (!(*iter1 < *j)) ++j;
            std::iter_swap(iter1, j);
            ++iter1;
            ++j;
            iter2 = k;
            std::rotate(iter1, j, last);
            while (last != j) ++j, ++iter2;
            std::rotate(k, iter2, last);
            return true;
        }
    }
    std::rotate(first, k, last);
    return false;
}

void next_combination_usage() {

    // Usage: next_combination on vector<int>
    vector<int> A = {5, 1, 3, 6, 2, 4};
    sort(A.begin(), A.end()); // Must sort first

    // The length of the array in the combination
    for (int comb_size = 1; comb_size <= 3; ++comb_size) {
        do {
            for (int i = 0; i < comb_size; i++) {
                cout << A[i] << ' ';
            }
            cout << endl;
        } while (next_combination(A.begin(), A.begin() + comb_size, A.end()));
    }

    // Usage: next_combination on string
    string s = "ashdioK";
    sort(s.begin(), s.end());

    size_t comb_size = 5;
    do {
        cout << string(s.begin(), s.begin() + comb_size) << endl;
    } while (next_combination(s.begin(), s.begin() + comb_size, s.end()));
}

// Longest Common Substrings: O(a.size() * b.size())
unordered_set<string> longestCommonSubstring(const string &a, const string &b) {

    unordered_set<string> result;
    if (a.empty() || b.empty()) return result;
    int r = (int)a.size();
    int n = (int)b.size();
    vector<vector<int> > dp(r, vector<int>(n));
    int z = 0;

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < n; ++j) {
            if (a.at(i) == b.at(j)) {
                if (i == 0 || j == 0) {
                    dp.at(i).at(j) = 1;
                } else {
                    dp.at(i).at(j) = dp.at(i-1).at(j-1)+1;
                }
                if (dp.at(i).at(j) > z) {
                    z = dp.at(i).at(j);
                    result.clear();
                    result.insert(a.substr(i-z+1,z));
                }
                else if (dp.at(i).at(j) == z) {
                    result.insert(a.substr(i-z+1,z));
                }
            }
            else {
                dp.at(i).at(j) = 0;
            }
        }
    }
    return result;
}

// Longest Common Subsequence: O(A.size()*B.size())
// assert( longestCommonSubseq("abcde", "bcef") == 3 );
auto longestCommonSubseq = [](const auto &A, const auto &B) {

    int len_A = static_cast<int>(A.size());
    int len_B = static_cast<int>(B.size());
    if (len_A == 0 || len_B == 0) {
        return 0;
    }
    vector<vector<int> > dp(len_A, vector<int>(len_B));
    for (int a = 0; a < len_A; a++) {
        for (int b = 0; b < len_B; b++) {
            if (a) dp[a][b] = max(dp[a][b], dp[a - 1][b]);
            if (b) dp[a][b] = max(dp[a][b], dp[a][b - 1]);
            if (A[a] == B[b])
                dp[a][b] = max(dp[a][b], ((a && b) ? dp[a - 1][b - 1] : 0) + 1);
        }
    }
    int ret = dp[len_A - 1][len_B - 1];
    return ret;
};

int LevenshteinDistance(const string &a, const string &b) {

    int a_length = static_cast<int>(a.size());
    int b_length = static_cast<int>(b.size());

    if (a_length == 0) return b_length;
    if (b_length == 0) return a_length;

    vector<vector<int> > dp(a_length + 1, vector<int>(b_length + 1));

    for (int i = 0; i <= a_length; ++i) {
        dp.at(i).at(0) = i;
    }
    for (int j = 0; j <= b_length; ++j) {
        dp.at(0).at(j) = j;
    }
    int cost;
    for (int i = 1; i <= a_length; ++i) {
        for (int j = 1; j <= b_length; ++j) {
            if (a.at(i - 1) == b.at(j - 1)) {
                cost = 0;
            } else {
                cost = 1;
            }
            dp.at(i).at(j) = min(
                    dp.at(i - 1).at(j) + 1,
                    min(dp.at(i).at(j - 1) + 1, dp.at(i - 1).at(j - 1) + cost)
            );
        }
    }
    return dp.at(a_length).at(b_length);
}

// Generate the prime table from 1 to MAXN
// Usage: Sieve primes from 1 to 1005: LinearPrimeSieve::init();
// Usage: Check if N is a prime: !LinearPrimeSieve::isNotPrime[N];
// Usage: Check the kth prime: LinearPrimeSieve::prime[k - 1];
//
// Reduce num_prime to the number of primes if the space is not enough.
namespace LinearPrimeSieve {

    const int MAXN = 1005;
    int prime[MAXN] = {0};
    int num_prime = 0;
    bool isNotPrime[MAXN] = {true, true};    // To store SPF(smallest prime factor), change it to int spf[MAXN] = {0, 0};

    // Generate the prime table from 1 to MAXN
    static void init() {
        for (int i = 2; i < MAXN; i++) {
            if (!isNotPrime[i])    // if (spf[i] == 0)
                prime[num_prime++] = i;
            for (int j = 0; j < num_prime && i * prime[j] < MAXN; j++) {    
                isNotPrime[i * prime[j]] = true;    // spf[i * prime[j]] = prime[j]
                if (!(i % prime[j]))
                    break;
            }
        }
    }
}

// Only check if n < MAXN is a prime
namespace EulerPrimeSieve_ {
    constexpr int MAXN = 100005;
    bitset<MAXN> isNotPrime;
    static void init() {
        isNotPrime[0] = true;
        isNotPrime[1] = true;
        for (int i = 2; i < MAXN; i++) {
            if (!isNotPrime[i]) {
                if (i > MAXN / i) {
                    continue;
                }
                for (int j = i * i; j < MAXN; j += i) {
                    isNotPrime[j] = true;
                }
            }
        }
    }
}

// Calculate a^n (n >= 0) in O(lgn) without recursion
ll quickPower(ll a, ll n) {
    ll res = 1;
    while (n > 0) {
        if (n & 1) {
            res *= a;
        }
        a *= a;
        n >>= 1;
    }
    return res;
}

bool isPrime(ll n) {
    if (n <= 1) {
        return false;
    }
    for (ll i = 2; i * i <= n; ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// Calculate (a^n) % m (n >= 0, m > 0) in O(lgn) without recursion
ll quickPower(ll a, ll n, ll m) {

    // Corner case:
    if (m == 1) {
        return 0;
    }

    // If m is a prime, then (a^n) % m = a^(n % (m - 1)) % m (Fermat's little theorem)
    if (isPrime(m)) {
        n = n % (m - 1);
    }

    a %= m;
    ll res = 1;
    while (n > 0) {
        if (n & 1) {
            res = res * a % m;
        }
        a = a * a % m;
        n >>= 1;
    }
    return res;
}

// O(1) modulo mul
ll multmod(ll a, ll b, ll m) {
    a = (a % m + m) % m;
    b = (b % m + m) % m;
    return ((a * b - (ll)((long double)(a) / m * b) * m) %
            m + m) % m;
}

// O(logb), better version
ll powmod(ll a, ll b, ll m) {
    if (m == 1) return 0;
    ll r;
    for (r = 1, a %= m; b; a = multmod(a, a, m), b >>= 1) {
        if (b % 2) r = multmod(r, a, m);
    }
    return r;
}

// a % p = a - floor(a / p) * p
// For a^b % p:
// If p is prime: a^b % p = a^(b % (p-1)) % p
// Otherwise: a^b % p = a^(b % phi(p) + phi(p)) % p

// A way to hash fixed length array
constexpr int MAXLENGTH = 26;

struct myHashFunc {
    size_t operator()(const array<int, MAXLENGTH> &A) const {
        size_t h = 0;
        for (const auto &i : A) {
            h ^= hash<int>{}(i) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

// Custom Hash for std::pair/std::tuple/std::array
// -------- mix & salt --------
constexpr std::uint64_t splitmix64(std::uint64_t x) noexcept {
    x += 0x9e3779b97f4a7c15ULL;
    x  = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x  = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
inline std::uint64_t global_salt() noexcept {
    static const std::uint64_t s =
            static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
    return s;
}
// -------- tuple-like detection (simple) --------
template<class T>
concept tuple_like = requires {
    std::tuple_size<std::remove_cvref_t<T>>::value;
    std::get<0>(std::declval<T>());
};
// -------- combine --------
template<class... Args>
std::size_t hash_combine_all(const Args&... xs) noexcept {
    std::uint64_t seed = global_salt();
    ((seed = splitmix64(seed ^ static_cast<std::uint64_t>(
            std::hash<std::remove_cvref_t<Args>>{}(xs)))), ...);
    return static_cast<std::size_t>(seed);
}
// -------- generic hash --------
template<class T>
struct CustomHash {
    std::size_t operator()(const T& v) const noexcept {
        if constexpr (tuple_like<T>) {
            return std::apply([](const auto&... e){ return hash_combine_all(e...); }, v);
        } else {
            return static_cast<std::size_t>(
                    splitmix64(global_salt() ^
                               static_cast<std::uint64_t>(std::hash<std::remove_cvref_t<T>>{}(v))));
        }
    }
};
// -------- Usage --------
void custom_hash_usage() {
    
    using Tup = std::tuple<int, int, std::string>;
    using Arr = std::array<int, 3>;
    using Pr = std::pair<int, std::string>;

    std::unordered_set<Tup, CustomHash<Tup>> st;
    st.emplace(1, 2, "asd");
    st.emplace(4, 5, "zxc");

    std::unordered_set<Arr, CustomHash<Arr>> sa;
    sa.insert({1, 2, 3});
    sa.insert({4, 5, 6});

    std::unordered_set<Pr, CustomHash<Pr>> sp;
    sp.emplace(7, "xx");
    sp.emplace(8, "yy");

    __gnu_pbds::gp_hash_table<int, std::pair<int, int>, CustomHash<int>> sp2;
    sp2[0] = {1, 2};
    sp2[1] = {2, 3};

    for (const auto &[a, b, c]: st) {
        std::cout << "tuple: " << a << ' ' << b << ' ' << c << '\n';
    }
    for (const auto &ar: sa) {
        std::cout << "array: ";
        for (auto x: ar) std::cout << x << ' ';
        std::cout << '\n';
    }
    for (const auto &[x, s]: sp) {
        std::cout << "pair : " << x << ' ' << s << '\n';
    }
    for (const auto &[k, v] : sp2) {
        std::cout << k << ':' << v.first << ',' << v.second << '\n';
    }
}

// max and min in double type with unknown number of arguments(at least 2)
constexpr double EPS = 1e-6;
inline constexpr static
int sgn(const double &x) {
    if (x > EPS) return 1;
    if (x > -EPS) return 0;
    return -1;
}
template <typename T = double>
inline const static
T & doubleMax(const T &a) {
    return a;
}
template<typename T = double>
inline const static
T & doubleMax(const T &l, const T &r) {
    return sgn(l - r) >= 0 ? l : r;
}
template<typename T = double, typename ... Args>
inline const static
T & doubleMax(const T &l, const Args &... args) {
    return doubleMax(l, doubleMax(args ...));
}
template <typename T = double>
inline const static
T & doubleMin(const T &a) {
    return a;
}
template<typename T = double>
inline const static
T & doubleMin(const T &l, const T &r) {
    return sgn(l - r) >= 0 ? r : l;
}
template<typename T = double, typename ... Args>
inline const static
T & doubleMin(const T& l, const Args& ... args) {
    return doubleMin(l, doubleMin(args ...));
}

// Reference:
// https://cs.uwaterloo.ca/~m32rober/rsqrt.pdf
// http://www.lomont.org/papers/2003/InvSqrt.pdf
//
// FAST INVERSE SQUARE ROOT in long double
double inv_sqrt64(double n) {
    double x2 = n * 0.5;
    double y = n;
    uint64_t i = *(uint64_t *) &y;
    i = 0x5fe6eb50c7b537a9 - (i >> 1);
    y = *(double *) &i;

    // add more Newton iterations if you wanna improve the accuracy
    y = y * (1.5 - (x2 * y * y));
    y = y * (1.5 - (x2 * y * y));
    //y = y * (1.5 - (x2 * y * y));
    //y = y * (1.5 - (x2 * y * y));
    return y;
}

// A collection of containers and procedures that implements Tarjan's strongly
// connected components algorithm. Assume that the node id starts from 1 and the
// index of a strongly connected component (SCC) also starts from 1
namespace SCC_Tarjan {

    // adjacency list of the graph
    vector<vector<int> > G;
    // number of nodes in the graph
    int number_of_nodes;
    // number of strongly connected components
    int number_of_scc;
    int current_timestamp;
    stack<int> s;   // A stack is used to store all nodes that may form a
    // strongly connected component
    vector<bool>
            vis; // vis.at(id) flags if the node id is in the stack
    vector<int> dfs_rank; // dfs_rank.at(id) numbers the nodes consecutively in
    // the order in which they are discovered by DFS
    vector<int> low_link; // low_link.at(id) represents the smallest node id of
    // any node known to be reachable from id through
    // id's DFS subtree, including id itself
    vector<int> scc;      // scc.at(id) is the index of the strongly connected
    // component that the node id belongs to
    vector<int> size_of_scc; // size_of_scc.at(id) is the size of the strongly
    // connected component whose index is id

    // reset all containers
    void reset() {
        decltype(G)().swap(G);
        stack<int>().swap(s);
        vector<bool>().swap(vis);
        vector<int>().swap(dfs_rank);
        vector<int>().swap(low_link);
        vector<int>().swap(scc);
        vector<int>().swap(size_of_scc);
    }

    // Initialize all global variables in the namespace
    void init(int n) {
        number_of_nodes = n;
        number_of_scc = 0;
        // NOTICE: starts from 1 if the node id starts from 1
        current_timestamp = 1;
        G.resize(number_of_nodes + 5);
        vis.resize(number_of_nodes + 5, false);
        dfs_rank.resize(number_of_nodes + 5, 0);
        low_link.resize(number_of_nodes + 5, 0);
        scc.resize(number_of_nodes + 5, 0);
        size_of_scc.resize(number_of_nodes + 5, 0);
    }

    void Tarjan(int u) { // u: the node id being processed
        dfs_rank.at(u) = current_timestamp;
        low_link.at(u) = current_timestamp;
        ++current_timestamp;
        s.push(u);
        vis.at(u) = true;
        for (const auto &v : G[u]) {
            if (!dfs_rank[v]) {
                Tarjan(v);
                low_link.at(u) = min(low_link.at(u), low_link.at(v));
            } else if (vis.at(v)) {
                low_link.at(u) = min(low_link.at(u), dfs_rank.at(v));
            }
        }
        if (low_link.at(u) == dfs_rank.at(u)) {
            ++number_of_scc;
            while (s.top() != u) {
                int top_id = s.top();
                // Paint top_id
                s.pop();
                scc.at(top_id) = number_of_scc;
                ++size_of_scc.at(number_of_scc);
                vis.at(top_id) = false;
            }
            // Paint u
            s.pop();
            scc.at(u) = number_of_scc;
            ++size_of_scc.at(number_of_scc);
            vis.at(u) = false;
        }
    }

    // Usage:
    int main() {
        int n, m, u, v;
        cin >> n >> m;
        init(n);
        while (m--) {
            cin >> u >> v;
            G[u].emplace_back(v);
        }
        // Run Tarjan's SCC algorithm
        for (int i = 1; i <= n; ++i) {
            // Process node i if the node i has not been visited
            if (!dfs_rank.at(i)) {
                Tarjan(i);
            }
        }
        return 0;
    }
}

// A collection of containers and procedures that implements the topological
// sort using Kahn's algorithm
namespace Toposort_Kahn {
    // adjacency list of the graph
    unordered_map<int, unordered_set<int, custom_hash>, custom_hash> G;
    int number_of_nodes;    // assume that the node id starts from 1
    // the number of incoming edges of each node, should be calculated during
    // the construction of the graph, not forget that the node id starts from 1
    vector<int> in_degree;
    vector<int> result; // store the result after toposort

    void reset() {
        G.clear();
        vector<int>().swap(in_degree);
        number_of_nodes = 0;
        vector<int>().swap(result);
    }

    void init(int n) {
        number_of_nodes = n;
        in_degree.resize(n + 5);
    }

    // Main logic of Kahn's algorithm: O(V+E), return true if it does not
    // have a cycle, otherwise false
    bool kahn() {
        queue<int> q;
        for (int i = 1; i <= number_of_nodes; ++i) {
            if (in_degree.at(i) == 0) {
                q.push(i);
            }
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            result.emplace_back(u);
            for (const auto &adj_v : G[u]) {
                --in_degree.at(adj_v);
                if (in_degree.at(adj_v) == 0) {
                    q.push(adj_v);
                }
            }
        }
        return (static_cast<int>(result.size()) == number_of_nodes);
    }
    // Kahn's algo if G is an undirected graph (also can be used to find nodes in the cycle)
    bool kahn2() {
        queue<int> q;
        vector<bool> vis(number_of_nodes+1);
        for (int i = 1; i <= number_of_nodes; ++i) {
            if (in_degree.at(i) <= 1) {
                q.push(i);
            }
        }
        int visCounter = 0;
        while (!q.empty()) {
            int u = q.front();
            vis[u] = true;
            q.pop();
            ++visCounter;
            for (const auto &adj_v : G[u]) {
                --in_degree.at(adj_v);
                if (in_degree.at(adj_v) == 1) {
                    q.push(adj_v);
                }
            }
        }
        // if vis[i] is false (i is not visited), then i is a node in cycles
        return visCounter == number_of_nodes;
    }

    // Main logic of Kahn's algorithm but the output should be lexicographically
    // smallest among all possible results after toposort: O(|V|log|V|+|E|)    
    bool kahn_with_lexicographically_smallest() {
        std::priority_queue<int, vector<int>, greater<> > q;
        for (int i = 1; i <= number_of_nodes; ++i) {
            if (in_degree.at(i) == 0) {
                q.push(i);
            }
        }
        while (!q.empty()) {
            int u = q.top();
            q.pop();
            result.emplace_back(u);
            for (const auto &adj_v : G[u]) {
                --in_degree.at(adj_v);
                if (in_degree.at(adj_v) == 0) {
                    q.push(adj_v);
                }
            }
        }
        return (static_cast<int>(result.size()) == number_of_nodes);
    }
}

void construct_the_graph() {
    Toposort_Kahn::init(6);
    Toposort_Kahn::G[6].insert(3);
    ++Toposort_Kahn::in_degree.at(3);
    Toposort_Kahn::G[6].insert(1);
    ++Toposort_Kahn::in_degree.at(1);
    Toposort_Kahn::G[5].insert(1);
    ++Toposort_Kahn::in_degree.at(1);
    Toposort_Kahn::G[5].insert(2);
    ++Toposort_Kahn::in_degree.at(2);
    Toposort_Kahn::G[3].insert(4);
    ++Toposort_Kahn::in_degree.at(4);
    Toposort_Kahn::G[4].insert(2);
    ++Toposort_Kahn::in_degree.at(2);
}


void test_for_toposort() {
    construct_the_graph();
    assert(Toposort_Kahn::kahn());
    Toposort_Kahn::reset();
    construct_the_graph();
    assert(Toposort_Kahn::kahn_with_lexicographically_smallest());
    vector<int> result = {5, 6, 1, 3, 4, 2};
    assert(Toposort_Kahn::result == result);
}

// A collection of containers and procedures that implements the topological
// sort using DFS
namespace Toposort_Dfs {
    // adjacency list of the graph
    unordered_map<int, unordered_set<int, custom_hash>, custom_hash> G;
    vector<int> vis;
    vector<int> result;

    bool dfs(int u) {
        vis[u] = -1;
        for (const auto &v : G[u]) {
            if (vis[v] < 0) {
                return false;
            }
            else if (vis[v] == 0) {
                if (!dfs[v]) {
                    return false;
                }
            }
        }
        vis[u] = 1;
        result.emplace_back(u);
        return true;
    }

    bool toposort(int number_of_nodes) {
        // reset
        vector<int>().swap(vis);
        vector<int>().swap(result);
        vis.resize(number_of_nodes);
        // start DFS from 0
        for (int i = 0; i < number_of_nodes; ++i) {
            if (vis[i] == 0) {
                if (!dfs(i)) {
                    return false;
                }
            }
        }
        reverse(result.begin(), result.end());
        return true;
    }
}

// The implementation of Dijkstra using adjacency list (based on the index)
namespace SSSP_Dijkstra_0 {
    constexpr int INF = 0x3f3f3f3f; //  A weight indicates two nodes have no paths between them
    struct Edge {
        int to, distance, next;

        bool operator==(const Edge &other) const {
            return to == other.to && distance == other.distance &&
                   next == other.next;
        }
    };

    struct Node {
        int distance, position;

        explicit Node(int distance, int position) : distance(distance),
                                                    position(position) {}

        bool operator<(const Node &other) const {
            return (other.distance < distance);
        }
    };

    // Containers to store the graph
    vector<Edge> E;
    vector<int> head;
    int counter;
    int number_of_nodes;
    int number_of_edges;

    vector<int> dis;    // dis.at(n) = the shortest distance from source to n
    vector<bool> vis;
    int source;         // the node id of the source
    vector<int> prev;   // an auxiliary container to store the path

    // __gnu_pbds::priority_queue<Node, less<>, pairing_heap_tag> pq; // Alternative option if you want to use pairing heap
    std::priority_queue<Node> pq;

    void init(int n, int e, bool store_path = false) {
        number_of_nodes = n;
        number_of_edges = e;
        // initialize the containers
        E.resize(number_of_edges + 5);
        head.resize(number_of_nodes + 5);
        dis.resize(number_of_nodes + 5, INF);
        vis.resize(number_of_nodes + 5);
        if (store_path) {
            prev.resize(number_of_nodes, -1);
        }
    }

    void reset(bool store_path = false) {
        fill(dis.begin(), dis.end(), INF);
        fill(vis.begin(), vis.end(), false);
        std::priority_queue<Node>().swap(pq);
        if (store_path) {
            fill(prev.begin(), prev.end(), -1);
        }
    }

    void add_edge(int u, int v, int w) {
        ++counter;
        E.at(counter).distance = w;
        E.at(counter).to = v;
        E.at(counter).next = head.at(u);
        head.at(u) = counter;
    }

    void
    dijkstra(bool store_path = false) {
        dis.at(source) = 0;
        pq.push(Node(dis.at(source), source));
        while (!pq.empty()) {
            Node temp_node = pq.top();
            pq.pop();
            int x = temp_node.position;
            if (vis.at(x)) {
                continue;
            }
            vis.at(x) = true;
            for (int i = head.at(x); i; i = E.at(i).next) {
                int y = E.at(i).to;
                if (dis.at(y) > dis.at(x) + E.at(i).distance) {
                    dis.at(y) = dis.at(x) + E.at(i).distance;
                    if (store_path) {
                        prev.at(y) = x;
                    }
                    if (!vis.at(y)) {
                        pq.push(Node(dis.at(y), y));
                    }
                }
            }
        }
    }

    vector<int>
    get_path(int destination) {
        vector<int> path;
        for (; destination != -1; destination = prev.at(destination)) {
            path.emplace_back(destination);
        }
        reverse(path.begin(), path.end());
        return path;
    }
}

// The implementation of Dijkstra using adjacency list
namespace SSSP_Dijkstra_1 {

    constexpr int INF = 0x3f3f3f3f; //  A weight indicates two nodes have no paths between them
    vector<vector<pair<int, int> > > adj; // The adjacency list of the graph
    int number_of_nodes;

    vector<int> dis;    // dis.at(n) = the shortest distance from source to n
    int source;         // the node id of the source
    vector<int> prev;   // an auxiliary container to store the path
    std::priority_queue<pair<int, int>, vector<pair<int, int> >, greater<> > pq;

    void
    init(int n, bool store_path = false) {
        number_of_nodes = n;
        // initialize the containers
        adj.resize(number_of_nodes + 5);
        dis.resize(number_of_nodes + 5, INF);
        if (store_path) {
            prev.resize(number_of_nodes, -1);
        }
    }

    void
    reset(bool store_path = false) {
        fill(dis.begin(), dis.end(), INF);
        decltype(pq)().swap(pq);
        if (store_path) {
            fill(prev.begin(), prev.end(), -1);
        }
    }

    void
    add_edge(int u, int v, int w) {
        adj.at(u).emplace_back(make_pair(v, w));
    }

    void
    dijkstra(bool store_path = false) {
        dis.at(source) = 0;
        pq.push(make_pair(dis.at(source), source));
        while (!pq.empty()) {
            int d_v = pq.top().first;
            int v = pq.top().second;
            pq.pop();
            if (d_v != dis.at(v)) {
                continue;
            }
            for (const auto &[to, weight] : adj.at(v)) {
                if (dis.at(to) > dis.at(v) + weight) {
                    dis.at(to) = dis.at(v) + weight;
                    if (store_path) {
                        prev.at(to) = v;
                    }
                    pq.push(make_pair(dis.at(to), to));
                }
            }
        }
    }

    vector<int>
    get_path(int destination) {
        vector<int> path;
        for (; destination != -1; destination = prev.at(destination)) {
            path.emplace_back(destination);
        }
        reverse(path.begin(), path.end());
        return path;
    }
}

// An implementation of Bellman Ford's algorithm
namespace SSSP_Bellman_Ford {
    constexpr int INF = 0x3f3f3f3f;
    vector<vector<pair<int, int> > > AL;
    vector<int> dist;
    int n, source;
    // Return false if there is a negative cycle
    bool bellman_ford() {
        dist.resize(n, INF);
        dist.at(source) = 0;
        bool relaxed;
        // relax all E edges V-1 times
        for (int i = 0; i < n - 1; ++i) {
            relaxed = false;
            for (int u = 0; u < n; ++u) {
                if (dist.at(u) != INF) {
                    for (const auto &[v, w] : AL.at(u)) {
                        // relax
                        if (dist.at(v) > dist.at(u) + w) {
                            dist.at(v) = dist.at(u) + w;
                            relaxed = true;
                        }
                    }
                }
                if (!relaxed) break;
            }
        }
        for (int u = 0; u < n; ++u) {
            if (dist.at(u) != INF) {
                for (const auto &[v, w] : AL.at(u)) {
                    if (dist.at(v) > dist.at(u) + w) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
}

// An implementation of Shortest Path Faster Algorithm (Bellman-Ford's algorithm with queue optimized)
// If you want to query the longest path from a single source, change INF to -INF and change the condition of relaxing from > to <
namespace SSSP_SPFA {
    constexpr int INF = 0x3f3f3f3f;
    vector<vector<pair<int, int> > > adj; // The adjacency list of the graph
    int source;
    int number_of_nodes;
    vector<int> dis;

    void
    reset() {
        decltype(adj)().swap(adj);
        vector<int>().swap(dis);
        number_of_nodes = 0;
        source = 0;
    }

    void
    init(int n, int s) {
        source = s;
        number_of_nodes = n;
        adj.resize(number_of_nodes + 5);
    }

    void
    add_edge(int u, int v, int w) {
        adj.at(u).emplace_back(make_pair(v, w));
    }

    // O(mn)
    bool
    spfa() {
        dis.resize(number_of_nodes + 5, INF);
        dis.at(source) = 0;
        vector<int> cnt(number_of_nodes + 5);
        vector<bool> in_queue(number_of_nodes + 5, false);
        queue<int> q;
        q.push(source);
        in_queue.at(source) = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            in_queue.at(u) = false;
            for (const auto &[v, w] : adj.at(u)) {
                if (dis.at(u) + w < dis.at(v)) {
                    dis.at(v) = dis.at(u) + w;
                    if (!in_queue.at(v)) {
                        q.push(v);
                        in_queue.at(v) = true;
                        ++cnt.at(v);
                        if (cnt.at(v) > number_of_nodes) {
                            return false;   // a negative cycle exists
                        }
                    }
                }
            }
        }
        return true;
    }
}

// Construction of a random antimagic-square from 1 to N * N
vector<vector<int> > antimagic_square(int n) {
    vector<vector<int> > res(n, vector<int>(n));
    int end_value = n * n - n + 1;
    int current_value = 1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            res.at(i).at(j) = current_value;
            ++current_value;
        }
        res.at(i).at(n - 1) = end_value;
        ++end_value;
    }
    return res;
}

// Pre-calculate the sum of factors of each number from 1 to n
vector<int> sum_of_factors(int n) {
    int length = n + 5;
    vector<int> result(length);  // result[k] = sum of factors of k
    for (int i = 1; i < length; ++i) {
        for (int j = 1; i * j < length; ++j) {
            result.at(i * j) += i;  // add the factor i
        }
    }
    return result;
}

// using Log2 = __lg2
template<typename F>
struct SparseTable {
    int n, maxLog;
    vector<int> Log2;
    vector<vector<ll>> spt;
    F op;
    SparseTable(int _n, F _op): n(_n), op(_op) {
        Log2.assign(n+1, 0);
        for (int i = 2; i <= n; ++i) Log2[i] = Log2[i/2] + 1;
        maxLog = Log2[n];
        spt.assign(n+1, vector<ll>(maxLog+1));
    }
    void input() {
        for (int i = 1; i <= n; ++i) cin >> spt[i][0];
    }
    void init() {
        for (int j = 1; j <= maxLog; ++j)
            for (int i = 1; i + (1 << j) - 1 <= n; ++i)
                spt[i][j] = op(spt[i][j-1], spt[i + (1 << (j-1))][j-1]);
    }
    ll query(int l, int r) const {
        int k = Log2[r - l + 1];
        return op(spt[l][k], spt[r - (1 << k) + 1][k]);
    }
};

//void SparseTableUsage() {
//    int n, m;
//    cin >> n >> m;
//    auto g = [](ll a, ll b){ return gcd(a, b); };
//    SparseTable<decltype(g)> st(n, g);
//    st.input();
//    st.init();
//    while (m--) {
//        int l, r;
//        cin >> l >> r;
//        cout << st.query(l, r) << "\n";
//    }
//    return 0;
//}

// Preprocess takes O(nmlog(n)log(m))
// Query index starts from 1
// The usage is similar to SparseTable 1D
// If the upperbound of n and m is fixed, use C style multiple dimension arrays instead!
struct SparseTable2D {
    vector<ll> Log2;
    vector<vector<vector<vector<ll>>>> spt;
    int n, m, logn, logm;

    void init() {
        int maxSize = max(n, m);
        Log2.resize(maxSize + 5);
        Log2[1] = 0;
        Log2[2] = 1;
        for (int i = 3; i < maxSize + 5; ++i)
            Log2[i] = Log2[i >> 1] + 1;
        logn = floor(log2(n) + 2);
        logm = floor(log2(m) + 2);
        spt.resize(n + 5);
        for (auto &i:spt) i.resize(m + 5);
        for (auto &i:spt) for (auto &j:i) j.resize(logn);
        for (auto &i:spt) for (auto &j:i) for (auto &k:j) k.resize(logm);
    }

    SparseTable2D() = default;

    explicit SparseTable2D(int n, int m) : n(n), m(m) {
        init();
    }

    // input 2d array of n*m elements
    void input() {
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= m; ++j) {
                cin >> spt.at(i).at(j).at(0).at(0);
            }
        }
    }

    void build() {
        for (int k = 0; k <= logn; ++k) {
            for (int f = 0; f <= logm; ++f) {
                if (k + f) {
                    for (int i = 1; i + (1 << k) - 1 <= n; ++i) {
                        for (int j = 1; j + (1 << f) - 1 <= m; ++j) {
                            if (k) {
                                spt[i][j][k][f] = max(spt[i][j][k - 1][f],
                                                      spt[i + (1 << (k - 1))][j][k - 1][f]);
                            } else {
                                spt[i][j][k][f] = max(spt[i][j][k][f - 1],
                                                      spt[i][j + (1 << (f - 1))][k][f - 1]);
                            }
                        }
                    }
                }
            }
        }
    }

    // Pre: r1 <= r2, c1 <= c2
    ll queryMax(int r1, int c1, int r2, int c2) {
        assert(r1 <= r2);
        assert(c1 <= c2);
        ll k1 = Log2.at(r2 - r1 + 1);
        ll k2 = Log2.at(c2 - c1 + 1);
        r2 = r2 - (int) (1 << k1) + 1;
        c2 = c2 - (int) (1 << k2) + 1;
        return max({spt[r1][c1][k1][k2], spt[r1][c2][k1][k2], spt[r2][c1][k1][k2], spt[r2][c2][k1][k2]});
    }
};

// Generate fixed-size array in compile-time
template<int N>
struct FibStruct {
    array<unsigned int, N> A;
    constexpr FibStruct() : A() {
        A.at(0) = 0;
        A.at(1) = 1;
        for (int i = 2; i < N; ++i) {
            A.at(i) = A.at(i - 1) + A.at(i - 2);
        }
    }
};
constexpr int MAXN = 30;
constexpr FibStruct<MAXN> s = FibStruct<MAXN>();
static_assert(s.A.at(MAXN - 1) == 514229);


// Twice DFS to query the diameter of a tree with different weight on each edge : O(n) 
namespace TreeDiameter0 {
    vector<vector<pair<int, int> > > adj;
    vector<int> d;
    int c = 0;

    void dfs(int u) {
        int v, w;
        for (const auto &i : adj.at(u)) {
            v = i.first;
            w = i.second;
            if (d.at(v) == 0) {
                d.at(v) = d.at(u) + w;
                if (d.at(v) > d.at(c)) {
                    c = v;
                }
                dfs(v);
            }
        }
    }

    int diameter() {
        d.at(0) = 1;    // node id starts from 0
        dfs(0);
        fill(d.begin(), d.end(), 0);
        d.at(c) = 1;
        dfs(c);
        return d.at(c) - 1;
    }

    void init(int n) {
        adj.resize(n + 5);
        d.resize(n + 5);
    }
}

// The maximum distances from every vertex are stored in maxDist[]
// Time cost: O(n)
namespace Eccentricity {
    constexpr int WEIGHT = 1;
    vector<vector<int> > AL;
    int n;
    vector<int> edged, maxDist;
    void init(int n_) {
        n = n_;
        decltype(AL)().swap(AL);
        vector<int>().swap(edged);
        vector<int>().swap(maxDist);
        edged.resize(n,-1);
        maxDist.resize(n);
        AL.resize(n);
    }
    // Obtain the longest path from the subtree with u as root to its leaves
    int dfs1(int u, int fa) {
        edged[u] = 0;
        for (const auto &v : AL[u]) {
            if (v == fa) {
                continue;
            }
            edged[u] = max(edged[u],WEIGHT+dfs1(v,u));
        }
        return edged[u];
    }
    void dfs2(int u, int fa, int pd) {
        int maxEdges[2] = {pd,0};
        int nwmg = 1;
        for (const auto &v : AL[u]) {
            if (v == fa) {
                continue;
            }
            if (edged[v]+1>maxEdges[0]) {
                maxEdges[1] = maxEdges[0];
                maxEdges[0] = edged[v]+WEIGHT;
                nwmg = 1;
            }
            else if (edged[v]+WEIGHT==maxEdges[0]) {
                ++nwmg;
            }
            else if (edged[v]+WEIGHT>maxEdges[1]) {
                maxEdges[1]=edged[v]+WEIGHT;
            }
        }
        for (const auto &v : AL[u]) {
            if (v == fa) {
                continue;
            }
            int upd = maxEdges[0];
            if (upd == edged[v]+1 && nwmg == 1) {
                upd = maxEdges[1];
            }
            dfs2(v,u,upd+WEIGHT);
        }
        maxDist[u] = max(pd, maxEdges[0]);
    }
    void run() {
        for (int i = 0; i < n; ++i) {
            if (edged[i] == -1) {
                dfs1(i,-1);
                dfs2(i,-1,0);
            }
        }
    }
}

struct Interval {
    int start;
    int end;
};
int solve(int n, vector<Interval>& Tree_edge, vector<int>& Edge_value) {
    // write code here
    TreeDiameter::init(n);
    int u, v, w;
    for (int i = 0; i < n - 1; ++i) {
        u = Tree_edge.at(i).start;
        v = Tree_edge.at(i).end;
        w = Edge_value.at(i);
        TreeDiameter::adj.at(u).emplace_back(make_pair(v, w));
        TreeDiameter::adj.at(v).emplace_back(make_pair(u, w));
    }
    return TreeDiameter::diameter();
}

// Obtain the tree diameter by DP
namespace TreeDiameter1 {
    // the longest distance each node can reach
    vector<int> d1;
    // the second longest distance each node can reach
    vector<int> d2;
    vector<vector<int> > adj;
    void init(int n) {
        adj.resize(n);
        d1.resize(n);
        d2.resize(n);
    }
    void dfs(int u, int father_of_u) {
        d1.at(u) = 0;
        d2.at(u) = 0;
        for (const auto &v : adj.at(u)) {
            if (v == father_of_u) continue;
            dfs(v, u);
            int temp = d1.at(v) + 1;
            if (temp > d1.at(u)) {
                d2.at(u) = d1.at(u);
                d1.at(u) = temp;
            } else if (temp > d2.at(u)) {
                d2.at(u) = temp;
            }
        }
    }
    // Usage:
    int main() {
        int n, m, u, v;
        cin >> n >> m;
        init(n);
        while (m--) {
            cin >> u >> v;
            adj[u].emplace_back(v);
            adj[v].emplace_back(u);
        }
        // randomly pick one node 
        // in the graph as root (we assume the tree is un-rooted)
        int dummy_root = 0;
        dfs(dummy_root, 0);
        int diameter = -1;
        for (int i = 0; i < n; ++i) {
            diameter = max(diameter, d1.at(i) + d2.at(i));
        }
        return 0;
    }
}

// Fib(2k) = Fib(k) * (2Fib(k+1) - Fib(k))
// Fib(2k+1) = Fib(k+1) * Fib(k+1) + F(k) * F(k)
template <typename T>
constexpr pair<T, T> fibInLog(int n) {
    assert(n >= 0);
    if (n == 0) {
        return {0, 1};
    }
    auto p = fibInLog<T>(n / 2);
    auto a = p.first * (2 * p.second - p.first);
    auto b = p.first * p.first + p.second * p.second;
    if (n % 2 != 0) {
        return {b, a + b};
    }
    return {a, b};
}
static_assert(fibInLog<ll>(80).first == 23416728348467685);

// Get the minimal rotation of the string
// Booth's algorithm : O(s.size())
int minimalRotation(string s) {
    s += s; // Concatenate string to it self to avoid modular arithmetic
    int length = (int)s.size();
    vector<int> f(length, -1);    // Failure function
    int k = 0;  // Least rotation of string found so far
    for (int j = 1, i; j < length; ++j) {
        char sj = s.at(j);
        i = f.at(j - k - 1);
        while (i != -1 && sj != s.at(k + i + 1)) {
            if (sj < s.at(k + i + 1)) {
                k = j - i - 1;
            }
            i = f.at(i);
        }
        if (sj != s.at(k + i + 1)) {
            if (sj < s.at(k)) {
                k = j;
            }
            f.at(j - k) = -1;
        } else {
            f.at(j - k) = i + 1;
        }
    }
    return k;
}
// Check if two strings are rotated
bool rotateString(string A, string B) {
    if (A.size() != B.size()) return false;
    rotate(A.begin(), A.begin() + minimalRotation(A), A.end());
    rotate(B.begin(), B.begin() + minimalRotation(B), B.end());
    return A == B;
}

// Stable Matching Problem
// Gale Shapley's algorithm
// O(mw)
namespace SMP {
    vector<queue<int> > m_pref;
    vector<vector<int> > w_pref;
    vector<int> engaged;    // w to m
    queue<int> free_man;
    vector<bool> is_woman_free;

    void reset() {
        vector<queue<int> >().swap(m_pref);
        vector<vector<int> >().swap(w_pref);
        vector<int>().swap(engaged);
        queue<int>().swap(free_man);
        vector<bool>().swap(is_woman_free);
    }

    void init(int n) {
        m_pref.resize(n + 1);   // man/woman's id starts from 1
        w_pref.resize(n + 1, vector<int>(n + 1));
        engaged.resize(n + 1);
        for (int i = 1; i <= n; ++i) {
            free_man.push(i);
        }
        is_woman_free.resize(n + 1, true);
    }

    void GaleShapley() {
        while (!free_man.empty()) {
            int m = free_man.front();
            if (m_pref.at(m).empty()) {
                free_man.pop();
                continue;
            }
            int w = m_pref.at(
                    m).front();   // first woman on m's list to whom m has not yet proposed
            if (is_woman_free.at(w)) {
                engaged.at(w) = m;
                m_pref.at(m).pop();
                free_man.pop();
                is_woman_free.at(w) = false;
            } else {
                int m_ = engaged.at(w);
                if (w_pref.at(w).at(m) >
                    w_pref.at(w).at(m_)) { // w prefers m to m_
                    free_man.push(m_);
                    engaged.at(w) = m;
                    free_man.pop();
                }
                m_pref.at(m).pop();
            }
        }
    }

    // n women, n men
    // men invite women
    // preferences of men to women will be given first, then the preferences of women to men
    // print the result of men to women
    int main() {
        int n, val;
        IO::read(n);
        SMP::init(n);
        // men
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j) {
                IO::read(val);
                SMP::m_pref.at(i).push(val);
            }
        }
        // women
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= n; ++j) {
                IO::read(val);
                SMP::w_pref.at(i).at(val) =
                        n + 1 - j;   // notice here
            }
        }
        SMP::GaleShapley();
        // print result: m to w
        vector<pair<int, int> > result(n);
        for (int i = 1; i <= n; ++i) {
            result.at(i - 1).first = i;
            result.at(i - 1).second = SMP::engaged.at(i);
            swap(result.at(i - 1).first, result.at(i - 1).second);
        }
        sort(result.begin(), result.end());
        IO::write(result.at(0).second);
        for (int i = 1; i < n; ++i) {
            putchar_unlocked(' ');
            IO::write(result.at(i).second);
        }
        putchar_unlocked('\n');
        return 0;
    }
}
// Given the preferences of 2n people and n pairs of matching of them, check
// how many unstable pairs among n pairs:
// https://leetcode.com/problems/count-unhappy-friends/

/** Pseudo-code
for w in women:
    for m in [men w would prefer over current_partner(w)]:
        if m prefers w to current_partner(m) return false
return true
**/

// Lucas's theorem
namespace Lucas {
    vector<ll> fac;
    // preprocess
    inline
    void init(ll p) {
        // reset
        vector<ll>().swap(fac);
        fac.emplace_back(1);
        for (ll i = 1; i <= p; ++i) {
            fac.emplace_back(fac.back() * i % p);
        }
    }
    inline static
    ll modpow(ll a, ll p, ll M) {
        if (M == 1) return 0;
        ll r;
        for (r = 1, a %= M; p; a = (a * a) % M, p >>= 1)
            if (p % 2)
                r = (r * a) % M;
        return r;
    }
    inline static
    ll C(ll n, ll m, ll p) {
        if (m > n) {
            return 0;
        }
        return fac[n] * modpow(fac[m] * fac[n - m], p - 2, p) % p;
    }
    // C(n, m) % p
    inline static
    ll Lucas(ll n, ll m, ll p) {
        if (m == 0) return 1;
        return (C(n % p, m % p, p) * Lucas(n / p, m / p, p)) % p;
    }
    // Given n, m, p such that p is a prime, calculate C(n+m,n) mod p
    int main() {
        int t;
        IO::read(t);
        ll n, m, p;
        while (t--) {
            IO::read(n, m, p);
            Lucas::init(p);
            IO::writeln(Lucas::Lucas(n + m, n, p));
        }
        return 0;
    }
}

// Lowest Common Ancestor - Tarjan's off-line algorithm
// O(n + m) for m queries
namespace LCA0 {
    vector<vector<int> > adj;
    vector<int> ancestor;
    vector<bool> vis;
    vector<vector<int> > queries;
    unordered_map<pii, int, custom_hash> unmap;
    void Tarjan(int u) {
        vis.at(u) = true;
        ancestor.at(u) = u;
        for (const auto &v : adj.at(u)) {
            if (!vis.at(v)) {
                Tarjan(v);
                DSU::merge(u, v);
                ancestor.at(DSU::find(u)) = u;
            }
        }
        for (const auto v : queries.at(u)) {
            if (vis.at(v)) {
                unmap[{u, v}] = ancestor.at(DSU::find(v));
                unmap[{v, u}] = ancestor.at(DSU::find(v));
            }
        }
    }
    void init(int n) {
        DSU::init(n);
        ancestor.resize(n + 5);
        vis.resize(n + 5, false);
        adj.resize(n + 5);
        queries.resize(n + 5);
    }
    // Usage:
    // n nodes, (n - 1) edges, the root is s, m queries,
    // for each query, u and v are given, print lca of u and v
    int main() {
        int n, m, s, u, v;
        IO::read(n, m, s);
        LCA0::init(n);
        for (int i = 1; i <= n - 1; ++i) {
            IO::read(u, v);
            LCA0::adj.at(u).emplace_back(v);
            LCA0::adj.at(v).emplace_back(u);
        }
        vector<pair<int, int> > query_list;
        for (int i = 1; i <= m; ++i) {
            IO::read(u, v);
            query_list.emplace_back(make_pair(u, v));
            LCA0::queries.at(u).emplace_back(v);
            LCA0::queries.at(v).emplace_back(u);
        }
        LCA0::Tarjan(s);    // Run Tarjan from root
        // output
        for (const auto &[u, v] : query_list) {
            IO::writeln(LCA0::unmap[{u, v}]);
        }
        return 0;
    }
}

// Obtain LCA by binary lifting, Preprocess O(nlogn), each query costs O(logn).
namespace LCA1 {
    // dfn: dfs序计数器
    int dfn,l,n;
    vector<int> dfsIn,dfsOut;
    vector<vector<int>> AL;
    vector<vector<int>> fa; //fa[x][y] = 节点x的第2^y个祖先
    void pre(int numberOfNodes) {
        vector<int>().swap(dfsIn);
        vector<int>().swap(dfsOut);
        decltype(fa)().swap(fa);
        decltype(AL)().swap(AL);
        n = numberOfNodes;
        dfn = 0;
        l = ceil(log2(n));
        dfsIn.resize(n);
        dfsOut.resize(n);
        AL.resize(n);
        fa.resize(n, vector<int>(l+1));
        vis.clear();
    }
    // 根据dfs出入序 可以确定u是否是v的祖先 作为祖先 必然先进后出
    bool isAncestorOf(int u, int v) {
        return dfsIn[u] <= dfsIn[v] && dfsOut[u] >= dfsOut[v];
    }
    void dfs(int u, int father) {
        dfsIn[u] = ++dfn;
        fa[u][0] = father;
        for (int i = 1; i <= l; ++i) {
            fa[u][i] = fa[fa[u][i-1]][i-1];
        }
        for (const auto &v : AL[u]) {
            if (v != father) dfs(v,u);
        }
        dfsOut[u] = ++dfn;
    }
    int lca(int u, int v) {
        if (isAncestorOf(u,v)) return u;
        else if (isAncestorOf(v,u)) return v;
        for (int i = l; i >= 0; --i) {
            if (!isAncestorOf(fa[u][i],v)) {
                u = fa[u][i];
            }
        }
        return fa[u][0];
    }
    void init(int root) {
        dfs(root,root);
    }
}

// Factorization of an integer using Miller Rabin Prime Check + Pollard Rho
namespace Factorization {

    constexpr int MAX_PRIME_FACTORS = 1000;
    ll factor[MAX_PRIME_FACTORS];    // Save the result of the factorization
    int tol;    // Count of prime factors

    // O(1)
    ll multmod(ll a, ll b, ll m) {
        a = (a % m + m) % m;
        b = (b % m + m) % m;
        return ((a * b - (ll) ((ld) a / m * b) * m) % m + m) % m;
    }

    // O(logb)
    ll powmod(ll a, ll b, ll m) {
        if (m == 1) return 0;
        ll r;
        for (r = 1, a %= m; b; a = multmod(a, a, m), b >>= 1)
            if (b % 2) r = multmod(r, a, m);
        return r;
    }

    // Runtime: O(logalogb)
    ll bingcd(ll a, ll b) {
        if (a < 0 || b < 0) return bingcd(abs(a), abs(b));
        if (!a || !b) return a | b;
        unsigned shift = __builtin_ctz(a | b);
        a >>= __builtin_ctz(a);
        do {
            b >>= __builtin_ctz(b);
            if (a > b) swap(a, b);
            b -= a;
        } while (b);
        return a << shift;
    }

    bool miller_rabin_subroutine(ll a, ll n, ll x, ll t) {
        ll result = powmod(a, x, n);
        ll last = result;
        for (int i = 1; i <= t; i++) {
            result = multmod(result, result, n);
            if (result == 1 && last != 1 && last != n - 1)
                return true;
            last = result;
        }
        return result != 1;
    }

    // O(Rlognlognlogn)
    bool miller_rabin(ll n) {
        constexpr int R = 5;   // rounds
        if (n < 2) return false;
        if (n == 2) return true;
        if ((n & 1) == 0) return false;
        ll x = n - 1, t = 0;
        ll a;
        while ((x & 1) == 0) {
            x >>= 1;
            ++t;
        }
        for (int i = 0; i < R; ++i) {
            a = rand() % (n - 1) + 1;
            if (miller_rabin_subroutine(a, n, x, t)) return false;
        }
        return true;
    }

    // Base on Brent's implementation, O(sqrt(p)) such that p is a small prime factor of n
    ll pollard_rho(ll n) {
        if (n % 2 == 0) return 2;
        if (n % 3 == 0) return 3;
        ll w = 0, a = 0, val = 1, g;
        ll c = rand() % (n - 1) + 1;
        for (ll k = 2;; k <<= 1, a = w, val = 1) {
            for (ll i = 1; i <= k; ++i) {
                w = (multmod(w, w, n) + c) % n;
                val = multmod(val, abs(w - a), n);
                if (! (i & 127)) {
                    g = bingcd(val, n);
                    if (g > 1) return g;
                }
            }
            g = bingcd(val, n);
            if (g > 1) return g;
        }
    }

    void find_prime_factors(ll n) {
        if (miller_rabin(n)) {
            factor[tol++] = n;
            return;
        }
        ll p = n;
        while (p >= n) p = pollard_rho(n);
        find_prime_factors(p);
        find_prime_factors(n / p);
    }
}

// Given a string s of length n, consisting only of lowercase English letters,
// find the number of different substrings in this string
int count_unique_substrings(string const &s) {

    /** This part can be done in compile-time **/
    int n = s.size();
    const int p = 31;
    const int m = 1e9 + 9;
    vector<ll> p_pow(n);
    p_pow[0] = 1;
    for (int i = 1; i < n; i++) {
        p_pow[i] = (p_pow[i - 1] * p) % m;
    }
    /*********************************/

    vector<ll> h(n + 1, 0);
    for (int i = 0; i < n; i++)
        h[i + 1] = (h[i] + (s[i] - 'a' + 1) * p_pow[i]) % m;

    int cnt = 0;
    for (int l = 1; l <= n; l++) {
        set<ll> hs;
        for (int i = 0; i <= n - l; i++) {
            ll cur_h = (h[i + l] + m - h[i]) % m;
            cur_h = (cur_h * p_pow[n - i - 1]) % m;
            hs.insert(cur_h);
        }
        cnt += hs.size();
    }
    return cnt;
}

// Given two strings - a pattern s and a text t, determine if the pattern
// appears in the text and if it does, enumerate all its occurrences in
// O(|s|+|t|) time
vector<int> rabin_karp(string const &s, string const &t) {

    /** Can be done in compile-time **/
    const int p = 31;
    const int m = 1e9 + 9;
    int S = s.size(), T = t.size();
    vector<long long> p_pow(max(S, T));
    p_pow[0] = 1;
    for (int i = 1; i < (int) p_pow.size(); i++)
        p_pow[i] = (p_pow[i - 1] * p) % m;
    /*********************************/

    vector<long long> h(T + 1, 0);
    for (int i = 0; i < T; i++)
        h[i + 1] = (h[i] + (t[i] - 'a' + 1) * p_pow[i]) % m;
    long long h_s = 0;
    for (int i = 0; i < S; i++)
        h_s = (h_s + (s[i] - 'a' + 1) * p_pow[i]) % m;

    vector<int> occurrences;
    for (int i = 0; i + S - 1 < T; i++) {
        long long cur_h = (h[i + S] + m - h[i]) % m;
        if (cur_h == h_s * p_pow[i] % m)
            occurrences.push_back(i);
    }
    return occurrences;
}

// Check whether an undirected graph is bipartite
// Principle: a graph is bipartite if and only if it is two-colorable (0/1)
namespace BipartiteCheck {
    constexpr int INF = 0x3f3f3f3f;
    vector<vector<int> > adj;
    vector<int> color;
    int n;
    void init(int number_of_nodes) {
        n = number_of_nodes;
        vector<int>().swap(color);
        color.resize(n + 5, INF);
    }
    // bfs from every node id, which will cover different CCs
    bool bfs() {
        queue<int> q;
        for (int s = 0; s < n; ++s) {
            if (color.at(s) == INF) {
                color.at(s) = 0;
                q.push(s);
            }
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                for (const auto &v : adj.at(u)) {
                    if (color.at(v) == INF) {
                        // if color[u] = 1, then color[v] = 0
                        // if color[u] = 0, then color[v] = 1
                        color.at(v) = 1 - color.at(u);
                        q.push(v);
                    } else if (color.at(v) == color.at(u)) {
                        // Coloring conflict found, exit
                        return false;
                    }
                }
            }
        }
        return true;
    }
    // Another version that bfs from 's', which means we only consider if the CC that contains 's' is bipartite
    bool bfs(int s) {
        bool isBipartite = true;
        queue<int> q;
        color.at(s) = 0;
        q.push(s);
        while (!q.empty() && isBipartite) {
            int u = q.front();
            q.pop();
            for (const auto &v : adj.at(u)) {
                if (color.at(v) == INF) {
                    // if color[u] = 1, then color[v] = 0
                    // if color[u] = 0, then color[v] = 1
                    color.at(v) = 1 - color.at(u);
                    q.push(v);
                } else if (color.at(v) == color.at(u)) {
                    // Coloring conflict found
                    isBipartite = false;
                    break;
                }
            }
        }
        return isBipartite;
    }
}

// Find cut vertices and bridges in an undirected graph in O(m+n)
namespace CutVertexAndBridges {

    int n; // number of nodes
    vector<vector<int>> adj; // adjacency list of graph
    vector<bool> visited;
    vector<bool> isCutVertex;
    vector<pair<int, int> > bridges;
    vector<int> tin, low;
    int timer;

    void reset() {
        decltype(adj)().swap(adj);
        decltype(visited)().swap(visited);
        decltype(isCutVertex)().swap(isCutVertex);
        decltype(bridges)().swap(bridges);
        decltype(tin)().swap(tin);
        decltype(low)().swap(low);
    }

    void init(int number_of_nodes) {
        n = number_of_nodes;
        adj.resize(n + 5);
        visited.resize(n + 5, false);
        isCutVertex.resize(n + 5, false);
        tin.resize(n + 5);
        low.resize(n + 5);
    }

    void dfs(int u, int p = -1) {
        visited[u] = true;
        tin[u] = low[u] = timer++;
        int children = 0;
        for (const auto v : adj[u]) {
            if (v == p) continue;
            if (visited[v]) {
                low[u] = min(low[u], tin[v]);
            } else {
                dfs(v, u);
                low[u] = min(low[u], low[v]);
                if (low[v] > tin[u]) {
                    if (u > v) {
                        bridges.emplace_back(v, u);
                    } else {
                        bridges.emplace_back(u, v);
                    }
                }
                if (low[v] >= tin[u] && p != -1)
                    isCutVertex[u] = true;
                ++children;
            }
        }
        if (p == -1 && children > 1)
            isCutVertex[u] = true;
    }

    void findCutVerticesAndBridges() {
        timer = 0;
        visited.resize(n, false);
        tin.resize(n, -1);
        low.resize(n, -1);
        isCutVertex.resize(n, false);
        vector<pair<int, int> >().swap(bridges);
        // Assume node id starts from 0
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) dfs(i);
        }
    }
}

// How to obtain edges biconnected Components (eBCC) in an undirected graph G
// 1. Identify all bridges in an undirected graph G
// 2. Remove all bridges in G, we have G'
// 3. Use DSU to get CCs in G'
// 4. Each CC in G' is an eBCC

// obtain vertice-biconnected components
// Notice: dont add self-loops in 'adj'
namespace vBCC {
    int n, bccCnt, dfnCnt;
    vector<vector<int> > adj, bcc;
    vector<int> dfn, low;
    stack<int> sta;
    void init(int number_of_nodes) {
        n = number_of_nodes;
        dfn.resize(n+5);
        low.resize(n+5);
        bcc.resize(n+5);
    }
    void Tarjan(int u, int fa) {
        ++dfnCnt;
        dfn[u] = dfnCnt;
        low[u] = dfnCnt;
        sta.push(u);
        if (adj[u].empty()) {
            bcc[++bccCnt].emplace_back(u);
            return;
        }
        int temp;
        for (const auto &v : adj[u]) {
            if (v == fa) continue;
            if (!dfn[v]) {
                Tarjan(v,u);
                low[u] = min(low[u],low[v]);
                if (low[v] >= dfn[u]) {
                    ++bccCnt;
                    do {
                        temp = sta.top();
                        sta.pop();
                        bcc[bccCnt].emplace_back(temp);
                    } while (temp != v);
                    bcc[bccCnt].emplace_back(u);
                }
            }
            else {
                low[u] = min(low[u], dfn[v]);
            }
        }
    }
    // node id starts from 1
    void run() {
        for (int i = 1; i <= n; ++i) {
            if (!dfn[i]) {
                Tarjan(i, -1);
            }
        }
    }
    // print all vbccs
    void print() {
        for (int i = 1; i <= bccCnt; ++i) {
            for (const int &j : bcc[i]) {
                cout << j << ' ';
            }
            cout << '\n';
        }
    }
}

namespace PrefixSum2D {
    vector<vector<ll> > pre;
    int n, m;

    void init(const vector<vector<ll> > &A) {

        if (A.empty() || A.front().empty()) return;
        n = (int) A.size();
        m = (int) A.front().size();
        decltype(pre)().swap(pre);
        pre.resize(n, vector<ll>(m));

        pre[0][0] = A[0][0];
        for (int i = 1; i < m; ++i)
            pre[0][i] = pre[0][i - 1] + A[0][i];
        for (int i = 1; i < n; ++i)
            pre[i][0] = pre[i - 1][0] + A[i][0];
        for (int i = 1; i < n; ++i)
            for (int j = 1; j < m; ++j)
                pre[i][j] = pre[i - 1][j] + pre[i][j - 1] - pre[i - 1][j - 1] + A[i][j];
    }

    ll rangeSum(int r0, int c0, int r1, int c1) {
        if (r0 == 0 && c0 == 0) return pre[r1][c1];
        else if (r0 == 0 && c0 != 0) return pre[r1][c1] - pre[r1][c0 - 1];
        else if (r0 != 0 && c0 == 0) return pre[r1][c1] - pre[r0 - 1][c1];
        return pre[r1][c1] - pre[r0 - 1][c1] - pre[r1][c0 - 1] + pre[r0 - 1][c0 - 1];
    }
}

// Prefix sum of edges' weights on a rooted tree
namespace PrefixSumTree0 {
    vector<ll> pre;
    vector<vector<pair<int, ll> > > adj;
    void init(int n) {
        adj.resize(n);
        pre.resize(n);
    }
    void dfs(int u, int father_of_u, ll currSum) {
        for (const auto &[v, w] : adj[u]) {
            if (v != father_of_u) {
                pre[v] = currSum + w;
                dfs(v, u, pre[v]);
            }
        }
    }
    // Query the distance from u to v:
    // ll dist(int u, int v) { return pre[u] + pre[v] - 2*pre[lca(u, v)]; }
    // Usage
    int main() {
        int n, root, m, u, v, q; ll w;
        cin >> n >> root >> m;
        init(n);
        while (m--) {
            cin >> u >> v >> w;
            adj[u].emplace_back(v, w);
            adj[v].emplace_back(u, w);
        }
        int dummy_node = -1;
        dfs(root, dummy_node, 0);
        while (q--) {
            cin >> u >> v;
            // cout << dist(u, v) << '\n';
        }
        return 0;
    }
}

// Prefix sum of nodes' weights on a rooted tree
namespace PrefixSumTree1 {
    vector<ll> pre;
    vector<vector<int> > adj;
    vector<ll> weight;
    vector<int> father;

    void init(int n) {
        pre.resize(n);
        adj.resize(n);
        weight.resize(n);
        father.resize(n);
    }

    void dfs(int u, int father_of_u, ll currSum) {
        pre[u] = currSum + weight[u];
        for (const auto &v : adj[u]) {
            if (v != father_of_u) {
                dfs(v, u, pre[u]);
                father[v] = u;
            }
        }
    }

    // Query the distance from u to v:
    //    ll dist(int u, int v) { 
    //        int Lca = lca(u, v);
    //        // Case 1: lca is root
    //        if (father[Lca] == -1) {
    //            return pre[u] + pre[v] - pre[Lca];
    //        }
    //        // Case 2: otherwise
    //        return pre[u] + pre[v] - pre[Lca] - pre[father[Lca]];
    //    }
    // Usage
    int main() {
        int n, root, m, u, v, q;
        cin >> n >> root >> m;
        init(n);
        for (auto &i : weight) cin >> i;
        while (m--) {
            cin >> u >> v;
            adj[u].emplace_back(v);
            adj[v].emplace_back(u);
        }
        father[root] = -1;
        int dummy_node = -1;
        dfs(root, dummy_node, 0);
        while (q--) {
            cin >> u >> v;
            // cout << dist(u, v) << '\n';
        }
        return 0;
    }
}

// C(n, m) = ((n-m+1)(n-m+2)...(n-m+m))/(m(m-1)(m-2)...2*1)
// P(m,k): For all integers m, k, such that k >= 1
// m(m+1)(m+2)...(m+k-1) is divided by k!
// This can be proved by nested induction
// Ref: 
// https://math.stackexchange.com/questions/12065/the-product-of-n-consecutive-integers-is-divisible-by-n-factorial/12073#12073
// https://gowers.wordpress.com/2010/09/18/are-these-the-same-proof/
ll comb(ll n, ll m) {
    if (m > n - m) m = n - m;
    ll result = 1;
    for (ll i = 1; i <= m; ++i) {
        result = result * (n - m + i) / i;
    }
    return result;
}

// Suppose there is a majority element in n integers such that its occurrence is > (n / 2)
// We can obtain in O(n) time and O(1) space
namespace MajorityElement {
    int main() {
        int val = -1, cnt = 0;
        while (n--) {
            scanf("%d", &a);
            if (a != val) {
                if (--cnt <= 0) {
                val = a, cnt = 1;
                }
            } else {
                ++cnt;
            }
        }
        cout << val << '\n';
    }
}

// Eulerian Circuit and Eulerian Path
// Eulerian Path: a trail in a finite graph that visits every EDGE exactly once
// Eulerian Circuit: an Eulerian trail that starts and ends on the same vertex
//
// For an undirected graph G:
// G has an Eulerian Circuit <=> G is connected, G has no vertices with odd degree
// G has an Eulerian Path <=> G is connected, G has 0 or 2 vertices with odd degree
// 
// For a directed graph G:
// G has an Eulerian Circuit <=> G is a single SCC and for all vertex v, in_degree[v] = out_degree[v]
// G has an Eulerian Path <=> Suppose its underlying undirected graph is G', for G'
// 1. G' is a single CC
// 2. there is at most 1 vertex v such that in_degree[v] - out_degree[v] = 1
// 3. there is at most 1 vertex v such that out_degree[v] - in_degree[v] = 1
// 4. for other vertices v, in_degree[v] = out_degree[v]
//
// We use Hierholzer's algorithm to find the Eulerian Circuit/Path
// 1. If we confirms G contains an Eulerian Path, then:
//    if G is undirected, 'startVertex' is one of the vertex with odd degree.
//    if G is directed, out_degree[startVertex] - in_degree[startVertex] = 1.
//
// 2. If we confirms G contains an Eulerian Circuit, then 'startVertex' can be any of vertices.
//
// Sort the adjList if you need to find the lexicographically smallest/largest Eulerian Path/Circuit

// Hierholzer for directed graphs
vector<int> Hierholzer(int startVertex, vector<deque<int> > &adjList) {
    stack<int> path;
    vector<int> circuit;
    int current = startVertex;
    path.push(current);
    while (!path.empty()) {
        if (!adjList.at(current).empty()) {
            path.push(current);
            int next = adjList.at(current).front();
            adjList.at(current).pop_front();
            current = next;
        } else {
            circuit.emplace_back(current);
            current = path.top();
            path.pop();
        }
    }
    reverse(circuit.begin(), circuit.end());
    return circuit;
}

// Hierholzer for undirected graphs
// Once an edge (u,v) is added to the graph
// We update 'stats' by ++stats[u][v], ++stats[v][u]
unordered_map<int, unordered_map<int, int> > stats;
vector<int> Hierholzer2(int startVertex, vector<deque<int> > &adjList) {
    stack<int> path;
    vector<int> circuit;
    int current = startVertex;
    path.push(startVertex);
    while (!path.empty()) {
        if (!adjList.at(current).empty()) {
            path.push(current);
            int next = adjList.at(current).front();
            adjList.at(current).pop_front();
            // avoid traversing the same edge twice
            if (stats[current][next] > 0) {
                --stats[current][next];
                --stats[next][current];
                current = next;
            }
        } else {
            circuit.emplace_back(current);
            current = path.top();
            path.pop();
        }
    }
    reverse(circuit.begin(), circuit.end());
    return circuit;
}

namespace Bitwise {
    // All boolean array are in 0-based indexing
    bool isOn(int S, int j) { return (S & (1 << j)); }
    void setBit(int &S, int j) { S |= (1 << j); }
    void clearBit(int &S, int j) { S &= ~(1 << j); }
    void toggleBit(int &S, int j) { S ^= (1 << j); }
    int lowBit(int S) { return S & (-S); }  // S&(-S) is 2^j such that the j-th of S is 0
    void setAll(int &S, int n) { S = (1<<n)-1; } // set S to all '1' bit of length n
    int modulo(int S, int n) { return S & (n - 1); } // Obtain S%n such that n is a power of 2
    bool isPowerOfTwo(int S) { return !(S & (S-1)); }
    int turnOffLastBit(int S) { return S & (S-1); } // pre: the last bit of S has not been turned off

    // __builtin_ctz(2^n) = n
    // Count how many bits are set in 'x'
    // __builtin_popcount(x)
    // __builtin_popcountl(x)
    // __builtin_ctz(x) return the number of trailing 0
    // __builtin_clz(x) return the number of leading 0

    // Loop through all subsets except itself and the empty set
    void loop_through_all_subsets(int s) {
        for (int ss=(s-1)&s; ss; ss=(ss-1)&s) {
            cout << bitset<5>(ss).to_string() << '\n';
        }
    }

    // Loop through all subsets with size r in N (Gosper's Hack)
    constexpr int N = 10;
    void loop_through_subsets_with_size_r(int r) {
        for (int s = (1<<r)-1; s<1<<N;) {
            // output
            cout << (bitset<N>)s << '\n';
            // update
            int x = s&(-s);
            int y = s+x;
            s = ((s&(~y))/x>>1)|y;
        }
    }

    // Compute the lexicographically next bit permutation
    // 0b10001 --> 0b10010
    // 0b00000 --> 0b11111 (32bits)
    unsigned int next_bit_permutation(unsigned current_bit_permutation) {
        unsigned int v = current_bit_permutation;
        unsigned int w; // next permutation of bits
        unsigned int t = v | (v - 1); // t gets v's least significant 0 bits set to 1
        // Next set to 1 the most significant bit to change,
        // set to 0 the least significant ones, and add the necessary 1 bits.
        w = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
        return w;
    }

    // Iterate all set bits in a 'bitset' instance
    void f() {
        bitset<10> bs;
        bs[0] = true;
        bs[9] = true;
        bs[5] = true;
        for (size_t i = bs._Find_first(); i < bs.size(); i = bs._Find_next(i))
            cout << i << endl;
    }
}

// 1D BIT
template<typename T>
struct BIT {
    vector<T> t1, t2;
    int N;
    int lowbit(int i) {
        return i & (-i);
    }
    // index starts from 1
    BIT(int n) : N(n+1) {
        t1.resize(N);
        t2.resize(N);
    }
    T sum(vector<T> &t, int n) {
        T a(0);
        while (n) {
            a += t[n];
            n -= lowbit(n);
        }
        return a;
    }
    T rsum(int n) {
        return sum(t1,n)*n-sum(t2,n);
    }
    T rsum(int l, int r) {
        return rsum(r)-rsum(l-1);
    }
    void upd(vector<T> &t, int n, T val) {
        while (n < N) {
            t[n] += val;
            n += lowbit(n);
        }
    }
    void rupd(int l, int r, T val) {
        upd(t1,l,val);
        upd(t1,r+1,-val);
        upd(t2,l,val*(l-1));
        upd(t2,r+1,-val*r);
    }
};

// 2D BIT
template<typename T>
struct BIT_2D {
    vector<vector<T> > t;
    T lowbit(T i) {
        return i & (-i);
    }
    int N, M;
    // index starts from 1
    BIT_2D() = default;
    BIT_2D(int n, int m) : N(n), M(m) {
        t.resize(N+1, vector<T>(M+1));
    }
    // add 'val' to t[x][y]
    void upd(int x, int y, T val) {
        for (int i = x; i <= N; i += lowbit(i)) {
            for (int j = y; j <= M; j += lowbit(j)) {
                t[i][j] += val;
            }
        }
    }
    // rangesum from [1][1] to [x][y]
    T sum(int x, int y) {
        T ans(0);
        for (int i = x; i > 0; i -= lowbit(i)) {
            for (int j = y; j > 0; j -= lowbit(j)) {
                ans += t[i][j];
            }
        }
        return ans;
    }
    T rsum(int x1, int y1, int x2, int y2) {
        return sum(x2,y2)-sum(x2,y1-1)-sum(x1-1,y2)+sum(x1-1,y1-1);
    }
};

// RMQ: Range Max/Min Query
// RSQ: Range Sum Query
struct SegmentTree {
    // max: INF = numeric_limits<int>::min();
    // min: INF = numeric_limits<int>::max();
    static const int INF = numeric_limits<int>::max();
    vector<int> st, lz;
    int n;
    void build(int p, int l, int r, const vector<int> &A) {
        if (l == r) { st[p] = A[l]; return; }
        build(2*p, l, (l+r)/2, A);
        build(2*p+1, (l+r)/2+1, r, A);
        st[p] = min(st[2*p], st[2*p+1]); // RMQ -> min/max, RSQ -> +
    }
    SegmentTree(vector<int> &A) {
        n = (int)A.size();
        st.resize(n << 2);
        lz.resize(n << 2);
        // The p is id of the tree, which starts from root = 1
        build(1, 0, n - 1, A);
    }
    void push(int p, int l, int r) {
        if (lz[p]) {
            st[p] = lz[p];
            // RMQ -> add: = lz[p],         increment: += lz[p]
            // RSQ -> add: = (r-l+1)*lz[p], increment: += (r-l+1)*lz[p]
            if(l!=r) lz[2*p] = lz[2*p+1] = lz[p]; // add: =, increment +=
            lz[p] = 0;
        }
    }
    int query(int p, int l, int r, int i, int j) {
        push(p, l, r);
        if (r < i or l > j) return INF; // RMQ -> INF, RSQ -> 0
        if (l >= i and r <= j) return st[p];
        return min(query(2*p, l, (l+r)/2, i, j),
                   query(2*p+1, (l+r)/2+1, r, i, j));
        // RMQ -> min/max, RSQ -> +
    }
    void update(int p, int l, int r, int i, int j, int v) {
        push(p, l, r);
        if (r < i or l > j) return;
        if (l >= i and r <= j) { lz[p] = v; push(p, l, r); return; }
        update(2*p, l, (l+r)/2, i, j, v);
        update(2*p+1, (l+r)/2+1, r, i, j, v);
        st[p] = min(st[2*p], st[2*p+1]); // RMQ -> min/max, RSQ -> +
    }
};
// Usage:
/*
int main() {
    vector<int> p = {6,1,1,5,6,1,9};
    SegmentTree st(p);
    // Range-update from p[1] to p[3] to 120
    st.update(1, 0, st.n - 1, 1, 3, 120);
    // 注意 当区间赋值为0时有问题 用其他值代替0
    // Query the minimal value from p[0] to p[4]
    cout << st.query(1, 0, st.n - 1, 0, 4) << '\n';
    return 0;
}
*/

// An implementation of Fast Fourier Transformation
namespace FFT {

    constexpr int LOG_MAX_LENGTH = 27;
    constexpr int MODULUS = 2013265921;
    constexpr int PRIMITIVE_ROOT = 137;
    constexpr int PRIMITIVE_ROOT_INVERSE = 749463956;

    int addMultiply(int x, int y, int z) {
        return (int)((x + y * static_cast<ll>(z)) % MODULUS);
    }

    void transform(const vector<int> &a, vector<int> &tA,
                   int logN, int primitiveRoot) {
        tA.resize(1 << logN);
        for (int j = 0; j < (int) a.size(); j++) {
            unsigned int k = j << (32 - logN); // MUST be unsigned
            k = ((k >> 1) & 0x55555555) | ((k & 0x55555555) << 1);
            k = ((k >> 2) & 0x33333333) | ((k & 0x33333333) << 2);
            k = ((k >> 4) & 0x0f0f0f0f) | ((k & 0x0f0f0f0f) << 4);
            k = ((k >> 8) & 0x00ff00ff) | ((k & 0x00ff00ff) << 8);
            tA[(k >> 16) | (k << 16)] = a[j];
        }
        int root[LOG_MAX_LENGTH];
        root[LOG_MAX_LENGTH - 1] = primitiveRoot;
        for (int i = LOG_MAX_LENGTH - 1; i > 0; i--) {
            root[i - 1] = addMultiply(0, root[i], root[i]);
        }
        for (int i = 0; i < logN; i++) {
            int twiddle = 1;
            for (int j = 0; j < (1 << i); j++) {
                for (int k = j; k < (int) tA.size(); k += 2 << i) {
                    int x = tA[k];
                    int y = tA[k + (1 << i)];
                    tA[k] = addMultiply(x, twiddle, y);
                    tA[k + (1 << i)] =
                            addMultiply(x, MODULUS - twiddle, y);
                }
                twiddle = addMultiply(0, root[i], twiddle);
            }
        }
    }

    void multiply (const vector<int>& a,
                   const vector<int>& b,
                   vector<int>& res) {
        int minN = int(a.size() - 1 + b.size());
        int logN = 0;
        while ((1 << logN) < minN) {
            logN++;
        }
        vector<int> tA, tB, nC;
        transform(a, tA, logN, PRIMITIVE_ROOT);
        transform(b, tB, logN, PRIMITIVE_ROOT);
        for (int j = 0; j < (int)tA.size(); j++)
            tA[j] = addMultiply(0, tA[j], tB[j]);
        transform(tA, nC, logN, PRIMITIVE_ROOT_INVERSE);
        res.resize(minN);
        int nInverse = MODULUS - ((MODULUS - 1) >> logN);
        for (int j = 0; j < minN; j++)
            res[j] = addMultiply(0, nInverse, nC[j]);
    }
    // Usage:
    int main() {
        int n, m;
        cin >> n >> m;
        vector<int> A(n);
        vector<int> B(m);
        for (auto &i : A) cin >> i;
        for (auto &i : B) cin >> i;
        vector<int> C;
        multiply(A, B, C);
        bool first_item = true;
        for (const auto &i : C) {
            cout << i << ' ';
        }
        cout << '\n';
        return 0;
    }
}

//#define BumpAllocator
#ifdef BumpAllocator
static char buf[450 << 20];
void* operator new(size_t s) {
    static size_t i = sizeof buf;
    assert(s < i);
    return (void*)&buf[i -= s];
}
void operator delete(void*) {}
#endif

// get string_view from a substring
string_view subStringView(
  string_view s, 
  size_t p, 
  size_t n = string_view::npos) {
  return s.substr(p, n);
}
// usage:
// string s = "abcd"; int i = 0, j = 2;
// string_view sv = subStringView(s, i, j-i+1);

/**
 * An implementation to use Zeller's congruence calculate the day of the week for any Julian or Gregorian calendar date
 * if the date is given. The function will not check if the given date is valid.
 */
string zellerCongruence(int year, int month, int day) {
    string result;
    bool flag = false;
    if (year <= 1581 || (year == 1582 && month <= 9) || (year == 1582 && month == 10 && month <= 4))
        flag = true;

    if (month == 1 || month == 2) {
        --year;
        month += 12;
    }
    int c = year / 100;
    int y = year - c * 100;
    int week = 0;

    if (!flag) {
        week = c / 4 - 2 * c + y + y / 4 + 13 * (month + 1) / 5 + day - 1;
        while (week < 0)
            week += 7;
        week %= 7;
    } else {
        week = y + y / 4 + c / 4 - 2 * c + 13 * (month + 1) / 5 + day + 2;
        while (week < 0)
            week += 7;
        week %= 7;
    }

    switch (week) {
        case 1:
            // printf("Monday\n");
            result = "Monday";
            break;
        case 2:
            // printf("Tuesday\n");
            result = "Tuesday";
            break;
        case 3:
            // printf("Wednesday\n");
            result = "Wednesday";
            break;
        case 4:
            // printf("Thursday\n");
            result = "Thursday";
            break;
        case 5:
            // printf("Friday\n");
            result = "Friday";
            break;
        case 6:
            // printf("Saturday\n");
            result = "Saturday";
            break;
        default:
            // printf("Sunday\n");
            result = "Sunday";
            break;
    }
    return result;
}
void zellerCongruenceTest() {
    assert(ZellerCongruence(2019, 8, 16) == "Friday");
    assert(ZellerCongruence(2019, 8, 17) == "Saturday");
    assert(ZellerCongruence(2019, 8, 15) == "Thursday");
    assert(ZellerCongruence(2019, 8, 14) != "Thursday");
    return 0;
}

// Maxflow
// Dinic: O(V^2*E)
// Edmonds Karp: O(V*E^2)
struct Maxflow {
    static constexpr ll INF = 4e18;
    using edge = tuple<int, ll, ll>;
    int n;
    vector<edge> el;
    vector<vector<int> > adj;
    vector<int> d, last;
    vector<pair<int, int> > p;
    bool bfs(int s, int t) {                       // find augmenting path
        d.assign(n, -1); d[s] = 0;
        queue<int> q({s});
        p.assign(n, {-1, -1});                       // record bfs sp tree
        while (!q.empty()) {
            int u = q.front(); q.pop();
            if (u == t) break;                         // stop as sink t reached
            for (auto &idx : adj[u]) {                  // explore neighbors of u
                auto &[v, cap, flow] = el[idx];          // stored in el[idx]
                if ((cap-flow > 0) && (d[v] == -1))      // positive residual edge
                    d[v] = d[u]+1, q.push(v), p[v] = {u, idx}; // 3 lines in one!
            }
        }
        return d[t] != -1;                           // has an augmenting path
    }

    ll send_one_flow(int s, int t, ll f = INF) {   // send one flow from s->t
        if (s == t) return f;                        // bottleneck edge f found
        auto &[u, idx] = p[t];
        auto &cap = get<1>(el[idx]), &flow = get<2>(el[idx]);
        ll pushed = send_one_flow(s, u, min(f, cap-flow));
        flow += pushed;
        auto &rflow = get<2>(el[idx ^ 1]);             // back edge
        rflow -= pushed;                             // back flow
        return pushed;
    }

    ll dfs(int u, int t, ll f = INF) {             // traverse from s->t
        if ((u == t) || (f == 0)) return f;
        for (int &i = last[u]; i < (int)adj[u].size(); ++i) { // from last edge
            auto &[v, cap, flow] = el[adj[u][i]];
            if (d[v] != d[u]+1) continue;              // not part of layer graph
            if (ll pushed = dfs(v, t, min(f, cap - flow))) {
                flow += pushed;
                auto &rflow = get<2>(el[adj[u][i] ^ 1]);     // back edge
                rflow -= pushed;
                return pushed;
            }
        }
        return 0;
    }

    Maxflow() = default;
    Maxflow(int number_of_nodes) : n(number_of_nodes) {
        adj.resize(n);
    }
    // if you are adding a bidirectional edge u<->v with weight w into your
    // flow graph, set directed = false (default value is directed = true)
    void add_edge(int u, int v, ll w, bool directed = true) {
        if (u == v) return;                          // safeguard: no self loop
        el.emplace_back(v, w, 0);                    // u->v, cap w, flow 0
        adj[u].push_back(el.size() - 1);                // remember this index
        el.emplace_back(u, directed ? 0 : w, 0);     // back edge
        adj[v].push_back(el.size() - 1);                // remember this index
    }
    ll edmonds_karp(int s, int t) {
        ll mf = 0;                                   // mf stands for max_flow
        while (bfs(s, t)) {                          // an O(V*E^2) algorithm
            ll f = send_one_flow(s, t);              // find and send 1 flow f
            if (f == 0) break;                       // if f == 0 stop
            mf += f;                                 // if f > 0, add to mf
        }
        return mf;
    }
    ll dinic(int s, int t) {
        ll mf = 0;                                   // mf stands for max_flow
        while (bfs(s, t)) {                          // an O(V^2*E) algorithm
            last.assign(n, 0);                         // important speedup
            while (ll f = dfs(s, t))                   // exhaust blocking flow
                mf += f;
        }
        return mf;
    }
};

// 如何将无权二分图最大匹配转换为最大流问题? 复杂度O(E*sqrt(V))
// Example: https://www.luogu.com.cn/problem/B3605
// 假设左侧有n个点 右侧有m个点
// 1. 将左侧点重新编号1到n
// 2. 将右侧点重新编号n+1到n+m
// 3. 构造虚假源点0和虚假汇点n+m+1
// 4. 源点到点1到n连一条容量为1的边
// 5. 点n+1到n+m连一条容量为1的边到汇点
// 6. 原来的每条从左往右的边容量也为1
// 7. 跑Dinic求最大流 即为最大匹配数

// Min Cost Max Flow, takes O(n^2m^2)
namespace MCMF {
    using edge = tuple<int, ll, ll, ll>;
    constexpr ll INF = 1e18;
    int n;
    ll totalCost;
    vector<edge> EL;
    vector<vector<int> > AL;
    vector<ll> d;
    vector<int> last;
    vector<bool> vis;

    void reset() {
        n = 0;
        totalCost = 0;
        decltype(EL)().swap(EL);
        decltype(AL)().swap(AL);
        decltype(d)().swap(d);
        decltype(last)().swap(last);
        decltype(vis)().swap(vis);
    }

    void init(int n_) {
        reset();
        n = n_;
        d.resize(n);
        vis.resize(n);
        AL.resize(n);
        last.resize(n);
    }

    // SPFA to find if there is an augmenting path
    // in residual graph
    bool spfa(int s, int t) {
        fill(d.begin(), d.end(), INF);
        d[s] = 0;
        vis[s] = true;
        queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            vis[u] = false;
            for (const auto &idx : AL[u]) {
                auto &[v, cap, flow, cost] = EL[idx];
                if (cap > flow && d[v] > d[u] + cost) {
                    d[v] = d[u] + cost;
                    if (!vis[v]) {
                        q.push(v);
                        vis[v] = true;
                    }
                }
            }
        }
        return d[t] != INF;
    }

    ll dfs(int u, int t, ll f = INF) {
        if (u == t || f == 0) {
            return f;
        }
        vis[u] = true;
        for (int &i = last[u]; i < (int) AL[u].size(); ++i) {
            auto &[v, cap, flow, cost] = EL[AL[u][i]];
            if (!vis[v] && d[v] == d[u] + cost) {
                if (ll pushed = dfs(v, t, min(f, cap - flow))) {
                    totalCost += pushed * cost;
                    flow += pushed;
                    auto &[rv, rcap, rflow, rcost]
                    = EL[AL[u][i] ^ 1];    // back edge
                    rflow -= pushed;
                    vis[u] = false;
                    return pushed;
                }
            }
        }
        vis[u] = false;
        return 0;
    }

    void addEdge(int u, int v, ll w, ll c,
                 bool directed = true) {
        if (u == v) {
            return;
        }
        EL.emplace_back(v, w, 0, c);
        AL[u].emplace_back((int) EL.size() - 1);
        EL.emplace_back(u, directed ? 0 : w, 0, -c);
        AL[v].emplace_back((int) EL.size() - 1);
    }

    pair<ll, ll> mcmf(int s, int t) {
        ll mf = 0;
        while (spfa(s, t)) {
            fill(last.begin(), last.end(), 0);
            while (ll f = dfs(s, t)) {
                mf += f;
            }
        }
        return {mf, totalCost};
    }

    // Usage: for a directed graph
    int main() {
        int n, m, s, t;
        cin >> n >> m >> s >> t;
        MCMF::init(n);
        int u, v;
        ll capacity, cost;
        while (m--) {
            cin >> u >> v >> capacity >> cost;
            MCMF::addEdge(u, v, capacity, cost);
        }
        auto ans = MCMF::mcmf(s, t);
        cout << ans.first << '\n';
        cout << ans.second << '\n';
        return 0;
    }
}

// 如果一个图是完全图 如何求出它的带权二分图最大匹配?
// 可以规约为最小费用最大流问题
// 1. 构造虚假源点s并连上所有左边的点 容量1费用0
// 2. 构造虚假汇点t并被所有右边的点连上 容量1费用0
// 3. 所有左边的点连右边的点的容量为1 费用为权重
// 4. 跑mcmf 最小费用即为原最大匹配数


// How to check if and only if an edge E is on the shortest 求边是否在u到v的最短路上
// path from u to v in an undirected graph:
//
// 1. Get dis1.at(i) = the shortest path from u to i by Dij (or the shortest path from i to u by Dij )
// 2. Get dis2.at(i) = the shortest path from v to i by Dij (or the shortest path from i to v by Dij )
// 3. For each edge E:(a, b) -> w,
// dis1.at(a) + w + dis2.at(b) = shortest path from u to v
// OR
// dis2.at(a) + w + dis1.at(b) = shortest path from u to v
// <=> the E:(a, b) -> w is on the shortest path from u to v

// 格雷码生成
// 1. k位格雷码从k个0开始
// 2. 翻转最低位得到下一个格雷码 比如000->001
// 3. 把最右边的1的左边翻位得到下一个格雷码 比如010->110
// 4. 重复2 3步 直到生成2^k-1个格雷码为止

// 求第n个格雷码
int g(int n) {
    return n ^ (n >> 1);
}

// 根据格雷码求其排名 从0开始
int rev_g(int g) {
    int n = 0;
    for (; g; g >>= 1) n ^= g;
    return n;
}

/**
 * An algorithm to calculate the number of factor p in factorial(n)
 * Pre-condition: p is a factor of factorial(n)
 * @return the number of factor p in n!
 */
inline static
ll countOfAFactorInFactorial(ll n, ll p) {
    ll result = 0;
    while (n != 0) {
        n /= p;
        result += n;
    }
    return result;
}


// Kruskal重构树
// https://leetcode.com/problems/checking-existence-of-edge-length-limited-paths-ii/
// https://codeforces.com/blog/entry/85714
namespace KruskalReconstructionTree {
    vector<vector<int>> AL; // 重构树
    vector<int> weight; // 重构树点权
    namespace DSU { // 没有启发式合并
        vector<int> father;
        void init(int n) {
            vector<int>().swap(father);
            father.resize(n+5);
            iota(father.begin(),father.end(),0);
        }
        int find(int x) {
            if (x != father[x]) {
                father[x] = find(father[x]);
            }
            return father[x];
        }
        void merge(int x,int y) {
            x = find(x);
            y = find(y);
            if (x == y) return;
            father[y] = x;
        }
        bool isSameGroup(int x,int y) {
            return find(x) == find(y);
        }
    }
    namespace LCA {
        constexpr int MAXN = 2e5+5;
        bitset<MAXN> vis;
        int dfn,l,n;
        vector<int> dfsIn,dfsOut;
        vector<vector<int>> fa;
        void pre(int numberOfNodes) {
            vector<int>().swap(dfsIn);
            vector<int>().swap(dfsOut);
            decltype(fa)().swap(fa);
            n = numberOfNodes;
            dfn = 0;
            l = ceil(log2(n));
            dfsIn.resize(n);
            dfsOut.resize(n);
            fa.resize(n, vector<int>(l+1));
            vis.reset();
        }
        bool isAncestorOf(int u, int v) {
            return dfsIn[u] <= dfsIn[v] && dfsOut[u] >= dfsOut[v];
        }
        void dfs(int u, int father) {
            vis[u] = true;
            dfsIn[u] = ++dfn;
            fa[u][0] = father;
            for (int i = 1; i <= l; ++i) {
                fa[u][i] = fa[fa[u][i-1]][i-1];
            }
            for (const auto &v : AL[u]) {
                if (v != father) dfs(v,u);
            }
            dfsOut[u] = ++dfn;
        }
        int lca(int u, int v) {
            if (isAncestorOf(u,v)) return u;
            else if (isAncestorOf(v,u)) return v;
            for (int i = l; i >= 0; --i) {
                if (!isAncestorOf(fa[u][i],v)) {
                    u = fa[u][i];
                }
            }
            return fa[u][0];
        }
        void init(int root) {
            dfs(root,root);
        }
    }
    struct Edge {
        int u,v,w;
        Edge() = default;
        Edge(int u,int v,int w) : u(u),v(v),w(w){}
        bool operator < (const Edge &e) const {
            return w < e.w;
        }
    }
    void construct(int n, vector<Edge> edgeList) {
        // 当求两点间所有简单路径上最小值的最大值 按边权从小到大排序
        // 当求两点间所有简单路径上最大值的最小值 按边权从大到小排序
        sort(edgeList.begin(),edgeList.end());
        int m = (int)edgeList.size();
        int m = (int)edgeList.size();
        weight.resize(n+m+1);
        AL.resize(n+m+1);
        DSU::init(n+m+1);
        int u,v,w,rootOfU,rootOfV;
        int id = n;
        // 构建Kruskal重构树
        for (const auto &i : edgeList) {
            u = i[0];
            v = i[1];
            w = i[2];
            rootOfU = DSU::find(u);
            rootOfV = DSU::find(v);
            if (rootOfU != rootOfV) {
                weight[id] = w;
                // 将两个集合的根作为新节点的左右子并合并
                DSU::merge(id, rootOfU);
                DSU::merge(id, rootOfV);
                AL[id].emplace_back(rootOfU);
                AL[rootOfU].emplace_back(id);
                AL[id].emplace_back(rootOfV);
                AL[rootOfV].emplace_back(id);
                ++id;
            }
        }
        LCA::pre(id);
        // 对所有未访问的节点进行dfs 避免森林的情况
        for (int i = 0; i < id; ++i) {
            // 从集合的根上进行dfs
            if (!LCA::vis[i]) LCA::init(DSU::find(i));
        }
    }
    // 求两点间所有简单路径上最小值的最大值 或 两点间所有简单路径上最大值的最小值
    int get(int u, int v) {
        // 两点不连通 返回-1
        if (DSU::find(u) != DSU::find(v)) {
            return -1;
        }
        return weight[LCA::lca(u,v)];
    }
}

// An implementation of Suffix Array with Induced-Sort
// Can construct SA in O(n)
namespace SA {
    // Constant booleans
    constexpr bool L_TYPE = false;
    constexpr bool S_TYPE = !L_TYPE;
    vector<int> sais(const vector<int> &s) {

        int n = (int) s.size() - 1;
        int maxValue = *max_element(s.begin(), s.end()) + 1;
        vector<int> SA(n + 1, -1);
        vector<int> bucket(maxValue), lbucket(maxValue), sbucket(maxValue);

        for (auto x : s) ++bucket[x];
        for (int i = 1; i < maxValue; ++i) {
            bucket[i] += bucket[i - 1];
            lbucket[i] = bucket[i - 1];
            sbucket[i] = bucket[i] - 1;
        }

        // Confirm the type and the position of '*'
        vector<bool> type(n + 1);
        type[n] = S_TYPE;
        for (int i = n - 1; i >= 0; --i) {
            type[i] = (s[i] < s[i + 1] ? S_TYPE : (s[i] > s[i + 1] ? L_TYPE : type[i + 1]));
        }

        // Define helper lambda functions
        auto is_lms_char = [&](int i) {
            return i > 0 && type[i] == S_TYPE && type[i - 1] == L_TYPE;
        };
        auto equal_substring = [&](int x, int y) {
            do {
                if (s[x] != s[y]) return false;
                ++x;
                ++y;
            } while (!is_lms_char(x) && !is_lms_char(y));
            return s[x] == s[y];
        };
        auto induced_sort = [&]() {
            for (int i = 0; i <= n; ++i) {
                if (SA[i] > 0 && type[SA[i] - 1] == L_TYPE) {
                    SA[lbucket[s[SA[i] - 1]]++] = SA[i] - 1;
                }
            }
            for (int i = 1; i < maxValue; ++i) {
                sbucket[i] = bucket[i] - 1;
            }
            for (int i = n; i >= 0; --i) {
                if (SA[i] > 0 && type[SA[i] - 1] == S_TYPE) {
                    SA[sbucket[s[SA[i] - 1]]--] = SA[i] - 1;
                }
            }
        };

        vector<int> pos;
        for (int i = 1; i <= n; ++i) {
            if (type[i] == S_TYPE && type[i - 1] == L_TYPE) {
                pos.emplace_back(i);
            }
        }
        for (auto x : pos) SA[sbucket[s[x]]--] = x;
        induced_sort();
        vector<int> name(n + 1, -1);
        int lx = -1, cnt = 0;
        bool flag = true;
        for (const auto &x : SA) {
            if (is_lms_char(x)) {
                if (lx >= 0 && !equal_substring(lx, x)) {
                    ++cnt;
                }
                if (lx >= 0 && cnt == name[lx]) {
                    flag = false;
                }
                name[x] = cnt;
                lx = x;
            }
        }
        vector<int> s1;
        for (const auto &x : name) {
            if (x != -1) {
                s1.emplace_back(x);
            }
        }
        vector<int> sa1;
        if (flag) {
            int n1 = s1.size();
            sa1.resize(n1);
            for (int i = 0; i < n1; ++i) sa1[s1[i]] = i;
        } else {
            sa1 = sais(s1);
        }
        lbucket[0] = sbucket[0] = 0;
        for (int i = 1; i < maxValue; ++i) {
            lbucket[i] = bucket[i - 1];
            sbucket[i] = bucket[i] - 1;
        }

        fill(SA.begin(), SA.end(), -1);
        for (int i = (int) sa1.size() - 1; i >= 0; --i) {
            SA[sbucket[s[pos[sa1[i]]]]--] = pos[sa1[i]];
        }
        induced_sort();
        return SA;
    }

    vector<int> sa;
    // Construct the suffix array for 'str'
    void init(const string &str) {
        vector<int> s(str.begin(), str.end());
        s.emplace_back(0);
        sa = sais(s);
        sa = vector<int>(sa.begin() + 1, sa.end());
    }
    // Find all occurrences of 'text' in 'pattern'
    void saFind(const string &pattern, const string &text) {
        auto r = equal_range(sa.begin(), sa.end(), -1, [&](int i, int j) {
            int a = 1;
            if (i == -1) {
                swap(i, j);
                a = -1;
            }
            return a * text.compare(i, pattern.size(), pattern) < 0;
        });
        vector occ(r.first, r.second);
        sort(occ.begin(), occ.end());
        for (const auto &i : occ) cout << i << ' ';
        cout << '\n';
    }
}

namespace Prufer {
    // 对树求prufer序列 节点从1开始
    vector<int> prufer_code(const vector<vector<int>>& AL) {
        int n = (int)AL.size();
        priority_queue<int, vector<int>, greater<>> leaves;
        vector<int> degree(n+1);
        vector<bool> removed(n+1, false);
        for (int i = 1; i <= n; ++i) {
            degree[i] = (int)AL[i].size();
            if (degree[i] == 1) {
                leaves.push(i);
            }
        }
        vector<int> ans;
        while (!leaves.empty()) {
            int u = leaves.top();
            leaves.pop();
            removed[u] = true;
            for (auto &v : AL[u]) {
                if (!removed[v]) {
                    ans.emplace_back(v);
                    --degree[v];
                    if (degree[v] == 1) {
                        leaves.push(v);
                    }
                }
            }
        }
        return ans;
    }
    // 根据Prufer序列重建树 节点从1开始
    vector<pair<int,int>> prufer_decode(const vector<int>& prufer_code) {
        vector<pair<int,int>> edgeList;
        int n = (int)prufer_code.size() + 1;
        vector<int> degree(n, 1);
        for (auto &i : prufer_code) {
            ++degree[i];
        }
        priority_queue<int, vector<int>, greater<>> leaves;
        for (int i = 1; i <= n; ++i) {
            if (degree[i] == 1) {
                leaves.push(i);
            }
        }
        for (auto &v : prufer_code) {
            int u = leaves.top();
            leaves.pop();
            edgeList.emplace_back(u, v);
            --degree[v];
            if (degree[v] == 1) {
                leaves.push(v);
            }
        }
        return edgeList;
    }
}

// coordinate compression (离散化)
vector<int> coordinateCompression(vector<int> a) {
    vector<int> b = a;
    sort(a.begin(),a.end());
    a.erase(unique(a.begin(),a.end()), a.end());
    int n = (int)b.size();
    for (int i = 0; i < n; ++i) {
        b[i] = lower_bound(a.begin(),a.end(),b[i]) - a.begin();
    }
    return b;
}

// 通过前缀异或和求子序列异或和 假设索引从1开始
// pre[0] = 0
// pre[i] = A[1] (i = 1) 
// pre[i] = pre[i-1]^A[i] (i >= 2)
// Then:
// If 2<=l<=r<=n, then
// pre[l-1]^pre[r] = (A[1]^A[2]^...^A[l-1])^(A[1]^A[2]^...^A[l-1]^A[l]^...^A[r])
// = A[l]^...^A[r]
namespace RangeXorSum {
    vector<int> pre;
    int rangeXorSum(int l, int r) {
        if (l == 1) {
            return pre[r];
        }
        return pre[l-1]^pre[r];
    }
    void init(const vector<int> &arr) {
        int n = (int)arr.size();
        vector<int>().swap(pre);
        pre.resize(n+1);
        for (int i = 1; i <= n; ++i) {
            pre[i] = pre[i-1]^arr[i-1];
        }
    }
}

// 高精度计算
namespace BigNum {
    string add(const string& a, const string& b) {
        int n = (int)max(a.size(), b.size()) + 1;
        string ans(n, '0');
        int i = (int)a.size() - 1;
        int j = (int)b.size() - 1;
        int k = 0;
        int x, y;
        while (i >= 0 || j >= 0) {
            x = (i >= 0) ? (a[i] - '0') : 0;
            y = (j >= 0) ? (b[j] - '0') : 0;
            ans[k] += x + y;
            if (ans[k] - '0' >= 10) {
                ++ans[k+1];
                ans[k] -= 10;
            }
            --i; --j; ++k;
        }
        if (ans.size() > 1 && ans.back() == '0') ans.pop_back();
        reverse(ans.begin(),ans.end());
        return ans;
    }
}

// Network Flow in O(V^2*sqrt(E))
// https://github.com/kth-competitive-programming/kactl/blob/main/kactl.pdf
struct PushRelabel {
    struct Edge {
        int dest, back;
        ll f, c;
    };
    vector<vector<Edge>> g;
    vector<ll> ec;
    vector<Edge *> cur;
    vector<vector<int>> hs;
    vector<int> H;

    explicit PushRelabel(int n) : g(n), ec(n), cur(n), hs(n<<1), H(n) {}

    void addEdge(int s, int t, ll cap, ll rcap = 0) {
        if (s == t) return;
        g[s].push_back({t, (int) g[t].size(), 0, cap});
        g[t].push_back({s, (int) g[s].size() - 1, 0, rcap});
    }

    void addFlow(Edge &e, ll f) {
        Edge &back = g[e.dest][e.back];
        if (!ec[e.dest] && f) hs[H[e.dest]].push_back(e.dest);
        e.f += f;
        e.c -= f;
        ec[e.dest] += f;
        back.f -= f;
        back.c += f;
        ec[back.dest] -= f;
    }

    ll calc(int s, int t) {
        int v = (int) g.size();
        H[s] = v;
        ec[t] = 1;
        vector<int> co(v << 1);
        co[0] = v - 1;
        for (int i = 0; i < v; ++i) cur[i] = g[i].data();
        for (Edge &e: g[s]) addFlow(e, e.c);
        for (int hi = 0;;) {
            while (hs[hi].empty()) if (!hi--) return -ec[s];
            int u = hs[hi].back();
            hs[hi].pop_back();
            while (ec[u] > 0)  // discharge u
                if (cur[u] == g[u].data() + (int) g[u].size()) {
                    H[u] = 1e9;
                    for (Edge &e: g[u])
                        if (e.c && H[u] > H[e.dest] + 1)
                            H[u] = H[e.dest] + 1, cur[u] = &e;
                    if (++co[H[u]], !--co[hi] && hi < v)
                        for (int i = 0; i < v; ++i)
                            if (hi < H[i] && H[i] < v)
                                --co[H[i]], H[i] = v + 1;
                    hi = H[u];
                } else if (cur[u]->c && H[u] == H[cur[u]->dest] + 1)
                    addFlow(*cur[u], min(ec[u], cur[u]->c));
                else ++cur[u];
        }
    }

    bool leftOfMinCut(int a) { return H[a] >= (int) g.size(); }
};

// Rolling hash 滚动哈希
namespace RH {
    constexpr ll MOD = 1e9+7;
    constexpr ll P = 31;
    unordered_set<ll> vis;
    vector<ll> Pow;
    vector<ll> Pow_inv;
    vector<ll> H;
    int n;
    ll multmod(ll a, ll b, ll m) {
        a = (a % m + m) % m;
        b = (b % m + m) % m;
        return ((a * b - (ll) ((long double) a / m * b) * m) % m + m) % m;
    }
    ll powmod(ll a, ll b, ll m) {
        if (m == 1) return 0;
        ll r;
        for (r = 1, a %= m; b; a = multmod(a, a, m), b >>= 1)
            if (b % 2) r = multmod(r, a, m);
        return r;
    }
    void pre(const string& s) {
        n = (int)s.size();
        Pow.resize(n);
        Pow_inv.resize(n);
        Pow[0] = 1;
        Pow_inv[0] = 1;
        for (int i = 1; i < n; ++i) {
            Pow[i] = (Pow[i-1] * P) % MOD;
            Pow_inv[i] = powmod(Pow[i], MOD - 2, MOD);
        }
        H.resize(n);
        ll h = 0;
        for (int i = 0; i < n; ++i) {
            h = (h + (s[i] - 'a' + 1) * Pow[i]) % MOD;
            H[i] = h;
        }
    }
    ll getHash(int l, int r) {
        if (l == 0) {
            return H[r];
        }
        return multmod(H[r] - H[l-1] + MOD, Pow_inv[l], MOD);
    }
    
}

// Trie树模版
constexpr int LETTERS = 26;
struct Node {
    Node* children[LETTERS];
    bool isEnd; // 表示是否存在以该节点作为结尾的单词 或者用个counter表示树中该单词出现的次数
    Node() {
        for (int i = 0; i < LETTERS; ++i) {
            children[i] = nullptr;
        }
        isEnd = false;
    }
};
struct Trie {
    Node* root;
    Trie() {
        root = new Node();
    }
    
    void insert(string word) {
        Node* node = root;
        int n = (int)word.size();
        for (int i = 0; i < n; ++i) {
            if (!node->children[word[i] - 'a']) {
                node->children[word[i] - 'a'] = new Node();
            }
            node = node->children[word[i] - 'a'];
        }
        node->isEnd = true;
    }
    
    bool search(string word) {
        Node* node = root;
        int n = (int)word.size();
        for (int i = 0; i < n; ++i) {
            if (!node->children[word[i] - 'a']) {
                return false;
            }
            node = node->children[word[i] - 'a'];
        }
        return node->isEnd;
    }
    
    bool startsWith(string prefix) {
        Node* node = root;
        int n = (int)prefix.size();
        for (int i = 0; i < n; ++i) {
            if (!node->children[prefix[i] - 'a']) {
                return false;
            }
            node = node->children[prefix[i] - 'a'];
        }
        return true;
    }
};

// 01 bfs (bfs with only two different weights)
namespace ZeroOneBFS {
    void bfs(int n, int s, const vector<pair<int,int>>& edges) {
        vector<int> d(n, INF);
        d[s] = 0;
        deque<int> dq;
        dq.push_front(s);
        while (!dq.empty()) {
            int v = dq.front();
            dq.pop_front();
            for (auto& edge : adj[v]) {
                int u = edge.first;
                int w = edge.second;
                if (d[v] + w < d[u]) {
                    d[u] = d[v] + w;
                    if (w == 1) // larger weights on the back
                        dq.push_back(u);
                    else
                        dq.push_front(u);
                }
            }
        }
    }
}

namespace ZeroOneKnapSack {
    constexpr int N = 500;
    constexpr int W_SUM = 1200;
    // 0-1 knapsack
    int memo[N+5][W_SUM+5];
    int W[N+5];
    int V[N+5];
    // dp(i, j) = Max value we can have under the current weight restrictions
    // consider all items from 0 to i-1
    int dp(int index, int remaining_weight) {
        if (index == N) {
            return 0;
        }
        else if (remaining_weight == 0) {
            return 0;
        }
        else if (memo[index][remaining_weight] != -1) {
            return memo[index][remaining_weight];
        }
        int result = -1;
        if (W[index] > remaining_weight) {
            result = dp(index + 1, remaining_weight);   // skip
            memo[index][remaining_weight] = result;
            return result;
        }
        result = max(dp(index + 1, remaining_weight), V[index] + dp(index + 1, remaining_weight - W[index])); // compare 'take it' and 'skip'
        memo[index][remaining_weight] = result;
        return result;
    }
}

// KMP Template adjusted from UofC_Notebook_2016
// Knuth-Morris-Pratt algorithm. Complexity: O (m + n)
// This function returns a vector containing the zero-based index of the start of each match of Pattern in Text. It works with strings,
// vectors, and pretty much any array-indexed data structure that has a size method. Matches may overlap.

template<class T>
vector<int> kmp(T const &Text, T const &Pattern) {

    vector<int> b(Pattern.size() + 1, -1);
    vector<int> matches;

    // Preprocess
    for (size_t i = 1; i <= Pattern.size(); i++) {
        int pos = b[i - 1];
        while (pos != -1 && Pattern[pos] != Pattern[i - 1])
            pos = b[pos];
        b[i] = pos + 1;
    }

    // Search
    int sp = 0, kp = 0;
    while (sp < Text.size()) {
        while (kp != -1 && (kp == Pattern.size() || Pattern[kp] != Text[sp])) {
            kp = b[kp];
        }
        ++kp;
        ++sp;
        if (kp == Pattern.size()) {
            matches.emplace_back(sp - Pattern.size());
        }
    }
    return matches;
}

void kmp_usage() {
    string A, B;
    getline(cin, A);
    getline(cin, B);
    auto res = kmp(A, B);
    if (res.empty()) {
        printf("-1");
    } else {
        bool firstItem = true;
        for (const int &i : res) {
            if (firstItem) {
                firstItem = false;
            } else {
                printf(" ");
            }
            printf("%d", i);
        }
    }
    printf("\n");
}

// Manacher’s algorithm: 在 O(n) 时间内找到第一个最长回文子串。
// 输入为 string_view，返回同一字符串上的 string_view（零拷贝）。
string_view manacher(string_view s) {
    // 在原始串两端和字符间插入分隔符，方便统一处理奇偶回文
    auto preprocess = [](string_view str) {
        if (str.empty()) return string("^$");
        string t;
        t.reserve(str.size() * 2 + 3);
        t.push_back('^');
        for (char c : str) {
            t.push_back('#');
            t.push_back(c);
        }
        t += "#$";
        return t;
    };

    std::string t = preprocess(s);
    int n = int(t.size());
    vector<int> p(n);     // p[i] = 以 i 为中心的回文半径
    int center = 0, right = 0; // 当前回文中心及其右边界

    // 构造回文半径数组
    for (int i = 1; i < n - 1; ++i) {
        int mirror = 2 * center - i;
        if (i < right) {
            p[i] = std::min(right - i, p[mirror]);
        }
        // 向两边扩展
        while (t[i + 1 + p[i]] == t[i - 1 - p[i]]) {
            ++p[i];
        }
        // 更新 center 和 right
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }
    }

    // 找到最大回文长度及其中心
    int max_center = 1;
    int max_len = p[1];
    for (int i = 2; i < n - 1; ++i) {
        if (p[i] > max_len) {
            max_len = p[i];
            max_center = i;
        }
    }

    // 计算原始字符串中的起始位置
    int start = (max_center - 1 - max_len) / 2;
    return s.substr(start, max_len);
}

void manacher_usage() {
    string input;
    getline(std::cin, input);
    string_view result = manacher(input);
    cout << result << '\n';
}

int main() {

    //freopen("in", "r", stdin);
    //freopen("out", "w", stdout);

    return 0;
}

// For unweight graphs, use BFS+vis array to find the shortest path between two nodes, dont use DFS+vis array which only works on weight graphs

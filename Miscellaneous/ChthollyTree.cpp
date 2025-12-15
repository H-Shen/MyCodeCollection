#include <algorithm>
#include <iostream>
#include <ranges>
#include <set>
#include <utility>
#include <vector>

using ll = long long;

class ChthollyTree {
    struct Node {
        int l;
        int r;
        mutable ll v;
        bool operator<(const Node& o) const { return l < o.l; }
    };

    std::set<Node> s_;
    int n_ = 0;

    /*
     * Split the segment at position pos (0-indexed).
     * Returns an iterator to the segment whose l == pos.
     *
     * Rules:
     * - If pos <= 0, returns begin().
     * - If pos >= n_, returns end() (no split).
     *
     * Precondition:
     * - s_ is a full partition of [0..n_-1] with non-overlapping segments.
     */
    std::set<Node>::iterator split_(int pos) {
        if (pos <= 0) return s_.begin();
        if (pos >= n_) return s_.end();

        auto it = s_.lower_bound(Node{pos, 0, 0LL});
        if (it != s_.end() && it->l == pos) return it;

        // pos must be inside the previous segment.
        --it;

        const int l = it->l;
        const int r = it->r;
        const ll v = it->v;

        s_.erase(it);
        s_.insert(Node{l, pos - 1, v});
        return s_.insert(Node{pos, r, v}).first;
    }

    /*
     * Normalize x into [0, mod).
     */
    static ll norm_mod_(ll x, ll mod) {
        x %= mod;
        if (x < 0) x += mod;
        return x;
    }

    /*
     * Portable overflow-safe (no __int128) multiplication: (a * b) % mod.
     * Uses binary multiplication with safe doubling under modulo.
     *
     * Requirement: mod > 0.
     */
    static ll mulmod_(ll a, ll b, ll mod) {
        a = norm_mod_(a, mod);
        b = norm_mod_(b, mod);

        ll res = 0;
        while (b > 0) {
            if (b & 1LL) {
                res += a;
                if (res >= mod) res -= mod;
            }
            // a = (2*a) % mod without signed overflow
            if (a >= mod - a) a = a - (mod - a);
            else a = a + a;
            b >>= 1LL;
        }
        return res;
    }

    /*
     * Fast exponentiation modulo mod using mulmod_().
     *
     * Requirement: mod > 0, exp >= 0.
     */
    static ll qpow_(ll base, ll exp, ll mod) {
        ll res = 1 % mod;
        base = norm_mod_(base, mod);
        while (exp > 0) {
            if (exp & 1LL) res = mulmod_(res, base, mod);
            base = mulmod_(base, base, mod);
            exp >>= 1LL;
        }
        return res;
    }

public:
    ChthollyTree() = default;

    /*
     * Build from a 0-indexed array a[0..n-1].
     * Each position becomes a singleton segment [i, i].
     */
    void build(const std::vector<ll>& a) {
        n_ = static_cast<int>(a.size());
        s_.clear();
        for (int i = 0; i < n_; ++i) {
            s_.insert(Node{i, i, a[static_cast<size_t>(i)]});
        }
    }

    /*
     * Add x to all values in [l, r] (0-indexed, inclusive).
     */
    void add(int l, int r, ll x) {
        const auto itr = split_(r + 1);
        const auto itl = split_(l);
        for (auto it = itl; it != itr; ++it) it->v += x;
    }

    /*
     * Assign x to all values in [l, r] (0-indexed, inclusive).
     */
    void assign(int l, int r, ll x) {
        auto itr = split_(r + 1);
        auto itl = split_(l);
        s_.erase(itl, itr);
        s_.insert(Node{l, r, x});
    }

    /*
     * Return the k-th smallest value in [l, r] (k is 1-based).
     * Returns -1 if k is invalid.
     */
    ll kth(int l, int r, int k) {
        const auto itr = split_(r + 1);
        const auto itl = split_(l);

        std::vector<std::pair<ll, int>> vec;
        vec.reserve(64);

        for (auto it = itl; it != itr; ++it) {
            vec.emplace_back(it->v, it->r - it->l + 1);
        }

        std::ranges::sort(vec);  // sort by (value, count)

        for (const auto& [val, cnt] : vec) {
            k -= cnt;
            if (k <= 0) return val;
        }
        return -1LL;
    }

    /*
     * Compute sum_{i=l..r} (a[i]^x) mod y.
     * Requirement: y > 0, x >= 0.
     */
    ll powsum(int l, int r, ll x, ll y) {
        const auto itr = split_(r + 1);
        const auto itl = split_(l);

        ll ans = 0;
        for (auto it = itl; it != itr; ++it) {
            const ll len = static_cast<ll>(it->r - it->l + 1) % y;
            const ll pw = qpow_(it->v, x, y);
            ans = (ans + mulmod_(len, pw, y)) % y;
        }
        return ans;
    }
};

/*
 * Deterministic PRNG used by the classic ODT demo.
 */
static ll g_seed = 0;

static ll rnd() {
    const ll ret = g_seed;
    g_seed = (g_seed * 7 + 13) % 1000000007LL;
    return ret;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n = 0;
    int m = 0;
    ll seed = 0;
    ll v_max = 0;

    // Demo input: n m seed v_max
    if (!(std::cin >> n >> m >> seed >> v_max)) return 0;
    g_seed = seed;

    // 0-indexed initial array: a[0..n-1]
    std::vector<ll> a(static_cast<size_t>(n));
    for (int i = 0; i < n; ++i) {
        a[static_cast<size_t>(i)] = (rnd() % v_max) + 1;
    }

    ChthollyTree tree;
    tree.build(a);

    // Random operations on 0-indexed ranges [l, r]:
    // 1 l r x : add x
    // 2 l r x : assign x
    // 3 l r k : kth smallest
    // 4 l r x y : sum(a[i]^x) mod y
    for (int i = 0; i < m; ++i) {
        const ll op = (rnd() % 4) + 1;

        int l = static_cast<int>(rnd() % n);
        int r = static_cast<int>(rnd() % n);
        if (l > r) std::swap(l, r);

        ll x = 0;
        ll y = 0;

        if (op == 3) x = (rnd() % (r - l + 1)) + 1;  // k in [1..len]
        else x = (rnd() % v_max) + 1;

        if (op == 4) y = (rnd() % v_max) + 1;

        if (op == 1) {
            tree.add(l, r, x);
        } else if (op == 2) {
            tree.assign(l, r, x);
        } else if (op == 3) {
            std::cout << tree.kth(l, r, static_cast<int>(x)) << '\n';
        } else {  // op == 4
            std::cout << tree.powsum(l, r, x, y) << '\n';
        }
    }

    return 0;
}

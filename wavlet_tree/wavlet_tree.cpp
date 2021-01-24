#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <time.h>
#include <tuple>
#include <vector>

using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::get;
using std::ostream;
using std::pair;
using std::priority_queue;
using std::string;
using std::tuple;
using std::vector;

#define DEBUG
#ifdef DEBUG
template <class T, class U>
ostream &operator<<(ostream &os, const std::pair<T, U> &p) {
    os << '(' << p.first << ',' << p.second << ')';
    return os;
}
template <class T> ostream &operator<<(ostream &os, const vector<T> &v) {
    os << '{';
    for(int i = 0; i < (int)v.size(); i++) {
        if(i) { os << ','; }
        os << v[i];
    }
    os << '}';
    return os;
}
void debugg() { cerr << endl; }
template <class T, class... Args>
void debugg(const T &x, const Args &... args) {
    cerr << " " << x;
    debugg(args...);
}
#define debug(...)                                                             \
    cerr << __LINE__ << " [" << #__VA_ARGS__ << "]: ", debugg(__VA_ARGS__)
#define dump(x) cerr << __LINE__ << " " << #x << " = " << (x) << endl
#else
#define debug(...) (void(0))
#define dump(x) (void(0))
#endif

class bit_vector {
  private:
    static const u16 L_SIZE = 512;
    static const u16 S_SIZE = 16;
    u32 n, l_len, s_len;
    vector<u16> B;
    vector<u32> L;
    vector<vector<u16>> S;

    int P(u32 x) { return __builtin_popcount(x); }

  public:
    bit_vector() {}
    bit_vector(const u32 n) : n(n) {
        l_len = (n + L_SIZE - 1) / L_SIZE;
        s_len = L_SIZE / S_SIZE;
        B.resize(l_len * s_len, 0);
        L.resize(l_len + 1, 0);
        S.resize(l_len, vector<u16>(s_len, 0));
    }
    // B[i] = b
    void set(const u32 i, const int b) {
        u32 b_id = i / S_SIZE;
        u32 k = i % S_SIZE;
        if(b == 0) {
            B[b_id] &= ~(1 << k);
        } else {
            B[b_id] |= (1 << k);
        }
    }
    // ！！！ かならずB[0]~B[n-1]までset()した後にこれを呼び出すこと ！！！
    void build() {
        L[0] = 0;
        for(int i = 0; i < l_len; i++) {
            S[i][0] = 0;
            for(int j = 0; j < s_len - 1; j++) {
                S[i][j + 1] = S[i][j] + P(B[i * s_len + j]);
            }
            L[i + 1] = L[i] + S[i][s_len - 1] + P(B[(i + 1) * s_len - 1]);
        }
    }
    // i番目の要素を返す
    u16 access(const u32 i) {
        u32 b_id = i / S_SIZE;
        u32 k = i % S_SIZE;
        return (B[b_id] >> k & 1);
    }
    // B[0, i)の中のb={0,1}の個数を返す
    u32 rank(const u32 i, const int b) {
        u32 l_id = i / L_SIZE;
        u32 b_id = i % L_SIZE / S_SIZE;
        u32 s_id = i % S_SIZE;

        u16 bit = B[l_id * s_len + b_id] & ((1 << s_id) - 1);
        int res = L[l_id] + S[l_id][b_id] + P(bit);
        return (b == 1 ? res : i - res);
    }
    // Bの中で、先頭から見てi番目に出現したb={0,1}のindexを返す
    u32 select(const u32 i, const int b) {
        u32 ok = n, ng = 0;
        while(ok - ng > 1) {
            u32 mid = (ok + ng) / 2;
            (rank(mid, b) >= i ? ok : ng) = mid;
        }
        return ok;
    }
    size_t size() { return (size_t)n; }
};

ostream &operator<<(ostream &os, bit_vector &bv) {
    os << '{';
    for(int i = 0; i < int(bv.size()); i++) {
        if(i) os << ',';
        os << bv.access(i);
    }
    os << '}';
    return os;
}

class wavlet_tree {
  private:
    struct Node {
        int l, r;
        Node *ch[2];
        Node *par;
        bit_vector bv;
        vector<int> dat;
        Node() : l(0), r(0), ch{nullptr, nullptr}, par(nullptr) {}
    };
    int n;
    int alp_size;
    Node *root;
    vector<int> dat;

    void debug_print(Node *now) {
        if(now == nullptr) return;
        debug(now->l, now->r);
        debug(now->dat);
        cerr << now->bv << endl;
        debug_print(now->ch[0]);
        debug_print(now->ch[1]);
    }
    bool is_leaf(Node *node) { return (node->r - node->l == 1); }
    bool is_root(Node *node) { return (node == root); }
    int left_or_right(Node *node, int x) {
        int mid = (node->r + node->l) / 2;
        if(node->l <= x && x < mid) return 0;
        return 1;
    }

  public:
    wavlet_tree(vector<int> v, int alp_sz)
        : n(v.size()), root(nullptr), dat(v), alp_size(alp_sz) {
        build();
    }
    void build() {
        root = new Node;
        auto dfs = [&](auto self, int l, int r, Node *now,
                       vector<int> &v) -> void {
            int mid = (l + r) / 2;
            vector<int> L, R;
            now->l = l, now->r = r;
            now->dat = v;
            now->bv = bit_vector(v.size());
            for(int i = 0; i < v.size(); i++) {
                if(v[i] < mid) {
                    L.push_back(v[i]);
                    now->bv.set(i, 0);
                } else {
                    R.push_back(v[i]);
                    now->bv.set(i, 1);
                }
            }
            now->bv.build();
            if(r - l == 1) return;
            if(mid - l > 0) {
                now->ch[0] = new Node;
                now->ch[0]->par = now;
                self(self, l, mid, now->ch[0], L);
            }
            if(r - mid > 0) {
                now->ch[1] = new Node;
                now->ch[1]->par = now;
                self(self, mid, r, now->ch[1], R);
            }
        };
        dfs(dfs, 0, alp_size, root, dat);
        // debug_print(root);
    }
    int access(int i) {
        int res = -1;
        auto dfs = [&](auto self, Node *now, int p) -> void {
            if(is_leaf(now)) {
                res = now->dat[0];
                return;
            }
            int b = now->bv.access(p);
            int np = now->bv.rank(p, b);
            self(self, now->ch[b], np);
        };
        dfs(dfs, root, i);
        return res;
    }
    int rank(int c, int i) {
        int res = -1;
        auto dfs = [&](auto self, Node *now, int p) -> void {
            if(is_leaf(now)) {
                res = p;
                return;
            }
            int b = left_or_right(now, c);
            int np = now->bv.rank(p, b);
            self(self, now->ch[b], np);
        };
        dfs(dfs, root, i);
        return res;
    }
    int select(int c, int i) {
        Node *leaf;
        auto get_leaf = [&](auto self, Node *now) -> void {
            if(is_leaf(now)) {
                leaf = now;
                return;
            }
            int b = left_or_right(now, c);
            self(self, now->ch[b]);
        };
        get_leaf(get_leaf, root);
        int res = -1;
        auto dfs = [&](auto self, Node *now, int p) -> void {
            if(is_root(now)) {
                res = p;
                return;
            }
            int b = left_or_right(now->par, now->l);
            int np = now->par->bv.select(p, b);
            self(self, now->par, np);
        };
        dfs(dfs, leaf, i + 1);
        return res - 1;
    }
    int qualtile(int s, int e, int rank) {
        int res = -1;
        auto dfs = [&](auto self, Node *now, int st, int en,
                       int remain) -> void {
            if(is_leaf(now)) {
                res = now->l;
                return;
            }
            int zerocnt = now->bv.rank(en, 0) - now->bv.rank(st, 0);
            if(remain < zerocnt) {
                int nxt_st = now->bv.rank(st, 0);
                int nxt_en = now->bv.rank(en, 0);
                self(self, now->ch[0], nxt_st, nxt_en, remain);
            } else {
                int nxt_st = now->bv.rank(st, 1);
                int nxt_en = now->bv.rank(en, 1);
                self(self, now->ch[1], nxt_st, nxt_en, remain - zerocnt);
            }
        };
        dfs(dfs, root, s, e, rank);
        return res;
    }
    vector<int> topk(int s, int e, int k) {
        using Data = tuple<Node *, int, int>;
        auto compare = [](const Data &a, const Data &b) {
            return (get<2>(a) - get<1>(a)) < (get<2>(b) - get<1>(b));
        };
        priority_queue<Data, vector<Data>, decltype(compare)> que{compare};
        que.emplace(root, s, e);
        vector<int> res;
        while(!que.empty() && (int)res.size() < k) {
            auto [now, st, en] = que.top();
            que.pop();
            if(is_leaf(now)) {
                res.push_back(now->l);
                continue;
            }
            for(int b = 0; b < 2; b++) {
                int bcnt = now->bv.rank(now->bv.size(), b);
                int ns = now->bv.select(1, b) - 1;
                int ne = now->bv.select(bcnt, b);
                que.emplace(now->ch[b], ns, ne);
            }
        }
        return res;
    }
    int rangefreq(int s, int e, int x, int y) {
        int res = rank(x, e) - rank(x, s);
        auto dfs_x = [&](auto self, Node *now, int st, int en) -> void {
            if(is_leaf(now)) return;
            int b = left_or_right(now, x);
            // 右の子のrankを求めて、resに足す
            int L, R;
            if(!is_root(now) && b == 0) {
                L = now->bv.rank(st, 1);
                R = now->bv.rank(en, 1);
                res += R - L;
            }
            L = now->bv.rank(st, b);
            R = now->bv.rank(en, b);
            self(self, now->ch[b], L, R);
        };
        auto dfs_y = [&](auto self, Node *now, int st, int en) -> void {
            if(is_leaf(now)) return;
            int b = left_or_right(now, y);
            // 左の子の範囲を求めて、resに足す
            int L, R;
            if(!is_root(now) && b == 1) {
                L = now->bv.rank(st, 0);
                R = now->bv.rank(en, 0);
                res += R - L;
            }
            L = now->bv.rank(st, b);
            R = now->bv.rank(en, b);
            self(self, now->ch[b], L, R);
        };
        dfs_x(dfs_x, root, s, e);
        dfs_y(dfs_y, root, s, e);
        if(y == alp_size) res += rank(y - 1, e) - rank(y - 1, s);
        return res;
    }
};

template <typename T> struct Compress {
    vector<T> v;
    Compress() {}
    Compress(vector<T> vv) : v(vv) {
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
    }
    int get(T x) {
        return (int)(std::lower_bound(v.begin(), v.end(), x) - v.begin());
    }
    T &operator[](int i) { return v[i]; }
    size_t size() { return v.size(); }
};

int main() {
    vector<int> v = {3, 0, 2, 0, 1, 3, 0, 1, 2, 1, 3, 2};
    wavlet_tree<4> wav(v);
    for(int i = 0; i < int(v.size()); i++) debug(wav.access(i));
    debug(wav.rank(0, 5));
    debug(wav.select(2, 1));
    debug(wav.qualtile(1, 5, 2));
    debug(wav.topk(0, 12, 3));
    debug(wav.rangefreq(0, 12, 0, 4));
}
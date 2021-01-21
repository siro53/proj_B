#include <assert.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::string;
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
    // B[0, i)の中の1の個数を返す
    u32 rank(const u32 i) {
        u32 l_id = i / L_SIZE;
        u32 b_id = i % L_SIZE / S_SIZE;
        u32 s_id = i % S_SIZE;

        u16 bit = B[l_id * s_len + b_id] & ((1 << s_id) - 1);
        return L[l_id] + S[l_id][b_id] + P(bit);
    }
    // Bの中で、先頭から見てi番目に出現した1のindexを返す
    u32 select(const u32 i) {
        u32 ok = n, ng = 0;
        while(ok - ng > 1) {
            u32 mid = (ok + ng) / 2;
            (rank(mid) >= i ? ok : ng) = mid;
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

template <int alp_size = 26> class wavlet_tree {
  private:
    struct Node {
        int l, r;
        Node *ch[2];
        bit_vector bv;
        vector<int> dat;
        Node() : l(0), r(0), ch{nullptr, nullptr} {}
    };
    int n;
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

  public:
    wavlet_tree(vector<int> v) : n(v.size()), root(nullptr), dat(v) { build(); }
    void build() {
        root = new Node;
        auto dfs = [&](auto self, int l, int r, Node *now,
                       vector<int> &v) -> void {
            if(r - l == 1) {
                now->l = l, now->r = r;
                now->dat.push_back(v[0]);
                now->bv = bit_vector(1);
                now->bv.set(0, 1);
                return;
            }
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
            if(mid - l > 0) {
                now->ch[0] = new Node;
                self(self, l, mid, now->ch[0], L);
            }
            if(r - mid > 0) {
                now->ch[1] = new Node;
                self(self, mid, r, now->ch[1], R);
            }
        };
        dfs(dfs, 0, alp_size, root, dat);
        debug_print(root);
    }
    int access(int i) {
        int res = -1;
        auto dfs = [&](auto self, Node *now, int p) -> void {
            if(now->r - now->l == 1) {
                res = now->dat[0];
                return;
            }
            int b = now->bv.access(p);
            int np = now->bv.rank(p);
            if(b == 0) np = p - np;
            self(self, now->ch[b], np);
        };
        dfs(dfs, root, i);
        return res;
    }
    int rank(int c, int i) {}
};

int main() {
    vector<int> v = {3, 0, 2, 0, 1, 3, 0, 1, 2, 1, 3, 2};
    wavlet_tree<4> wav(v);
    for(int i = 0; i < int(v.size()); i++) debug(wav.access(i));
}
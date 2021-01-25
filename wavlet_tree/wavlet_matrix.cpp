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

class wavlet_matrix {
  private:
    unsigned int height;
    vector<bit_vector> B;
    vector<int> pos;
    // val の i ビット目の値を返す(0,1)
    int get(const int val, const int i) { return val >> i & 1; }

  public:
    wavlet_matrix() {}
    wavlet_matrix(vector<int> dat)
        : wavlet_matrix(dat, *std::max_element(dat.begin(), dat.end()) + 1) {}
    // alp_size:文字の種類数
    wavlet_matrix(vector<int> dat, const unsigned int alp_size) {
        build(dat, alp_size);
    }
    void build(vector<int> &dat, const unsigned int alp_size) {
        height = (alp_size == 1) ? 1 : (64 - __builtin_clzll(alp_size - 1));
        B.resize(height), pos.resize(height);
        for(unsigned int i = 0; i < height; i++) {
            B[i] = bit_vector(dat.size());
            for(unsigned int j = 0; j < dat.size(); j++) {
                B[i].set(j, get(dat[j], height - i - 1));
            }
            B[i].build();
            auto it = std::stable_partition(dat.begin(), dat.end(), [&](int c) {
                return !get(c, height - i - 1);
            });
            pos[i] = it - dat.begin();
        }
    }
    // [0, i) の間に現れる値 val の数
    int rank(int val, int i) {
        int p = 0;
        for(unsigned int j = 0; j < height; ++j) {
            if(get(val, height - j - 1)) {
                p = pos[j] + B[j].rank(p, 1);
                i = pos[j] + B[j].rank(i, 1);
            } else {
                p = B[j].rank(p, 0);
                i = B[j].rank(i, 0);
            }
        }
        return i - p;
    }
};
#define TESTCASE_NUM 5

int main() {
    double sum = 0;
    for(int t = 0; t < TESTCASE_NUM; t++) {
        string filename = "test/random" + std::to_string(t + 1) + ".in";
        std::ifstream In(filename);
        int n, asz;
        In >> n >> asz;
        vector<int> dat(n);
        for(int i = 0; i < n; i++) In >> dat[i];
        wavlet_matrix wt(dat, asz);
        int q;
        In >> q;
        double total = 0;
        for(int _ = 0; _ < q; _++) {
            int r, c;
            In >> r >> c;
            clock_t start = clock();
            int res = wt.rank(c, r);
            clock_t end = clock();
            total += double(end - start);
        }
        total /= q;
    }
    sum /= TESTCASE_NUM;
    printf("rank() time: %lf sec\n", sum / CLOCKS_PER_SEC);
}
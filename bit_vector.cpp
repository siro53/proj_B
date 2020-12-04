#include <bits/stdc++.h>

using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

class bit_vector {
  private:
    static const u16 L_SIZE = 512;
    static const u16 S_SIZE = 16;
    u32 n, l_len, s_len;
    std::vector<u16> B;
    std::vector<u32> L;
    std::vector<std::vector<u16>> S;

    int P(u32 x) { return __builtin_popcount(x); }

  public:
    bit_vector(const u32 n) : n(n) {
        l_len = (n + L_SIZE - 1) * L_SIZE;
        s_len = L_SIZE / S_SIZE;
        B.resize(l_len * s_len, 0);
        L.resize(l_len + 1, 0);
        S.resize(l_len, std::vector<u16>(s_len, 0));
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
                S[i][j + 1] = S[i][j] + P(B[i * l_len + j]);
            }
            L[i + 1] = L[i] + S[i][s_len - 1] + P(B[(i + 1) * l_len - 1]);
        }
    }
    // i番目の要素を返す
    u16 access(const u32 i) {
        u32 b_id = i / L_SIZE;
        u32 k = i % L_SIZE;
        return (B[b_id] >> k & 1);
    }
    // B[0, i)の中の1の個数を返す
    u32 rank(const u32 i) {
        u32 l_id = i / L_SIZE;
        u32 b_id = i % L_SIZE / S_SIZE;
        u32 s_id = i % S_SIZE;

        u16 bit = B[l_id * l_len + b_id] & ((1 << s_id) - 1);
        return L[l_id] + S[l_id][b_id] + P(bit);
    }
    // Bの中で、先頭から見てi+1番目に出現した1のindexを返す
    u32 select(const u32 i) {
        u32 ok = n, ng = 0;
        while(ok - ng > 1) {
            u32 mid = (ok + ng) / 2;
            (rank(mid) >= i ? ok : ng) = mid;
        }
        return ok;
    }
};

int main() {}
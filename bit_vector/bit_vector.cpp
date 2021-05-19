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
using std::string;
using std::vector;

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
};

#define TESTCASE_NUM 5

int main() {
    double access_time = 0;
    double rank_time = 0;
    double select_time = 0;
    double start, end;

    for(int t = 1; t <= TESTCASE_NUM; t++) {
        string filename = "test/random" + std::to_string(t) + ".in";
        std::ifstream In(filename);
        string s;
        In >> s;
        int n = int(s.size());
        bit_vector bv(n);
        int cnt = 0;
        for(int i = 0; i < n; i++) {
            bv.set(i, s[i] - '0');
            cnt += (s[i] == '1');
        }
        bv.build();
        // access
        start = clock();
        for(int i = 0; i < n; i++) u16 tmp = bv.access(i);
        end = clock();
        access_time += double(end - start);
        // rank
        start = clock();
        for(int i = 0; i < n; i++) u32 tmp = bv.rank(i);
        end = clock();
        rank_time += double(end - start);
        // select
        start = clock();
        for(int i = 1; i <= cnt; i++) u16 tmp = bv.select(i);
        end = clock();
        select_time += double(end - start);
        printf("testcase%d finished.\n", t);
    }
    access_time /= TESTCASE_NUM;
    rank_time /= TESTCASE_NUM;
    select_time /= TESTCASE_NUM;

    printf("access() time: %lf sec\n", double(access_time / CLOCKS_PER_SEC));
    printf("rank() time: %lf sec\n", double(rank_time / CLOCKS_PER_SEC));
    printf("select() time: %lf sec\n", double(select_time / CLOCKS_PER_SEC));
}
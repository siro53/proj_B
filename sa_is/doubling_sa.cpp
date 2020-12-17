#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::ifstream;
using std::string;
using std::vector;

#define TESTCASE_NUM 20

class doubling_SA {
  private:
    string S;
    int n, k;
    vector<int> sa, tmp, rank;

  public:
    doubling_SA(string s)
        : S(s), n(s.size()), sa(n + 1), tmp(n + 1), rank(n + 1) {
        for(int i = 0; i <= n; i++) {
            sa[i] = i;
            rank[i] = (i < n ? S[i] : -1);
        }

        auto compare_sa = [&](int l, int r) {
            if(rank[l] != rank[r]) return rank[l] < rank[r];
            int ri = (l + k <= n ? rank[l + k] : -1);
            int rj = (r + k <= n ? rank[r + k] : -1);
            return ri < rj;
        };

        for(k = 1; k <= n; k *= 2) {
            std::sort(sa.begin(), sa.end(), compare_sa);
            tmp[sa[0]] = 0;
            for(int i = 1; i <= n; i++) {
                tmp[sa[i]] =
                    tmp[sa[i - 1]] + (compare_sa(sa[i - 1], sa[i]) ? 1 : 0);
            }
            for(int i = 0; i <= n; i++) rank[i] = tmp[i];
        }
    }
    int operator[](const int i) { return sa[i + 1]; }
};

int main() {
    double sum = 0;
    for(int i = 0; i < TESTCASE_NUM; i++) {
        string filename = "test/random" + std::to_string(i + 1) + ".in";
        ifstream In(filename);
        string s;
        In >> s;
        clock_t start = clock();
        doubling_SA sa(s);
        clock_t end = clock();
        sum += (double)(end - start);
    }
    sum /= TESTCASE_NUM;
    printf("doubling_SA time: %lf sec\n", sum / CLOCKS_PER_SEC);
}
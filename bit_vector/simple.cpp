#include <assert.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <vector>

using std::string;
using std::vector;

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
        vector<int> rank(n);
        int cnt = 0;
        for(int i = 0; i < n; i++) {
            rank[i + 1] = (s[i] == '1');
            cnt += (s[i] == '1');
        }
        for(int i = 0; i < n; i++) rank[i + 1] += rank[i];
        // access
        start = clock();
        for(int i = 0; i < n; i++) int tmp = rank[i + 1] - rank[i];
        end = clock();
        access_time += double(end - start);
        // rank
        start = clock();
        for(int i = 0; i < n; i++) int tmp = rank[i];
        end = clock();
        rank_time += double(end - start);
        // select
        start = clock();
        for(int i = 1; i <= cnt; i++) {
            int ok = n, ng = 0;
            while(ok - ng > 1) {
                int mid = (ok + ng) / 2;
                (rank[mid] >= i ? ok : ng) = mid;
            }
        }
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
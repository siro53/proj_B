#include <bits/stdc++.h>
using namespace std;
#define repl(i, a, b) for(int i = (a); i < int(b); i++)
#define rep(i, n) repl(i, 0, n);

class SA_IS {
  private:
    vector<int> sa_is(const vector<int> &t, int alp_sz) {
        const int n = int(t.size());
        vector<bool> is_S(n, false), is_LMS(n, false);
        vector<int> LMSs;

        // type-Sなsuffixを調べる
        is_S[n - 1] = true;
        for(int i = n - 2; i >= 0; i--) {
            if(t[i] < t[i + 1] || ((t[i] == t[i + 1]) && is_S[i + 1])) {
                is_S[i] = true;
            }
        }

        // type-S*なsuffixを調べる
        for(int i = 0; i < n; i++) {
            if(is_S[i] && (i == 0 || !is_S[i - 1])) {
                is_LMS[i] = true;
                LMSs.push_back(i);
            }
        }

        // まずtype-S*がソートされていると仮定して、induced_sortを行う。
        vector<int> pre_SA = induced_sort(t, LMSs, is_S, alp_sz);
        vector<int> sorted_LMSs(LMSs.size(), 0);
        {
            int id = 0;
            for(int i : pre_SA) {
                if(is_LMS[i]) sorted_LMSs[id++] = i;
            }
        }

        int rank = 0;
        pre_SA[sorted_LMSs[0]] = 0;
        if(int(sorted_LMSs.size()) > 1) pre_SA[sorted_LMSs[1]] = ++rank;
        for(int i = 1; i < int(LMSs.size()) - 1; i++) {
            bool is_diff = false;
            for(int j = 0; j < n; j++) {
                int now = sorted_LMSs[i] + j;
                int nxt = sorted_LMSs[i + 1] + j;
                if(t[now] != t[nxt] || is_LMS[now] != is_LMS[nxt]) {
                    is_diff = true;
                    break;
                }
                if(j > 0 && is_LMS[now]) break;
            }
            if(is_diff) {
                pre_SA[sorted_LMSs[i + 1]] = ++rank;
            } else {
                pre_SA[sorted_LMSs[i + 1]] = rank;
            }
        }
        vector<int> new_t(LMSs.size());
        {
            int id = 0;
            for(int i = 0; i < n; i++) {
                if(is_LMS[i]) {
                    new_t[id] = pre_SA[i];
                    id++;
                }
            }
        }
        vector<int> new_sa(LMSs.size(), 0);
        if(rank + 1 == int(LMSs.size())) {
            new_sa = sorted_LMSs;
        } else {
            new_sa = sa_is(new_t, rank + 1);
            for(int i = 0; i < int(new_sa.size()); i++) {
                new_sa[i] = LMSs[new_sa[i]];
            }
        }

        auto res = induced_sort(t, new_sa, is_S, alp_sz);
        return res;
    }

    vector<int> induced_sort(const vector<int> &t, const vector<int> &LMSs,
                             const vector<bool> &is_S, const int alp_sz) {
        int n = int(t.size());
        vector<int> SA(n, 0);
        vector<int> C(alp_sz + 1, 0);
        for(int i = 0; i < n; i++) C[t[i] + 1]++;
        for(int i = 0; i < alp_sz; i++) C[i + 1] += C[i];

        // まずtype-S*からバケットに詰める。各bucketにS*を後ろから挿入。
        vector<int> cnt(alp_sz, 0);
        for(int i = int(LMSs.size()) - 1; i >= 0; i--) {
            int c = t[LMSs[i]];
            SA[C[c + 1] - 1 - cnt[c]] = LMSs[i];
            cnt[c]++;
        }

        // 次にtype-Lを詰める。SAを前から走査する。
        // type-S*であるsuffix
        // S[i]について、S[i-1]がtype-Lならバケットの先頭に入れる
        cnt.assign(alp_sz, 0);
        for(int i = 0; i < n; i++) {
            if(SA[i] == 0 || is_S[SA[i] - 1]) continue;
            int c = t[SA[i] - 1];
            SA[C[c] + cnt[c]] = SA[i] - 1;
            cnt[c]++;
        }

        // 最後にtype-Sを詰める。SAを後ろから走査する
        // type-Lであるsuffix
        // S[i]について、S[i-1]がtype-Sならバケットの後ろに入れる
        cnt.assign(alp_sz, 0);
        for(int i = n - 1; i >= 0; i--) {
            if(SA[i] == 0 || !is_S[SA[i] - 1]) continue;
            int c = t[SA[i] - 1];
            SA[C[c + 1] - 1 - cnt[c]] = SA[i] - 1;
            cnt[c]++;
        }

        return SA;
    }

  public:
    string S;
    vector<int> sa;
    SA_IS(string s) : S(s), sa(S.size()) {
        int n = int(s.size());
        s += '$';
        vector<int> t(n + 1);
        for(int i = 0; i < n; i++) t[i] = s[i] - '$';
        sa = sa_is(t, 100);
        sa.erase(sa.begin());
    }
    int operator[](const int i) const { return sa[i]; }
};

int main() {
    string s;
    cin >> s;
    SA_IS sa(s);
    int n = int(s.size());
    for(int i = 0; i < n; i++) cout << sa[i] << " \n"[i + 1 == n];
}
#include <algorithm>
#include <fstream>
#include <random>
#include <string>

#define TESTCASE_NUM 5

int randint(int l, int r) {
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::uniform_int_distribution<int> dist(l, r);
    return dist(engine);
}

void shuffle(std::vector<int> &v) {
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    std::shuffle(v.begin(), v.end(), engine);
}

int main() {
    int n = 300000;
    for(int t = 0; t < TESTCASE_NUM; t++) {
        std::string filename = "test/random" + std::to_string(t + 1) + ".in";
        std::ofstream of(filename);
        int alp_size = 200000;
        of << n << " " << alp_size << "\n";
        std::vector<int> dat;
        for(int i = 0; i < n; i++) {
            if(i < alp_size) {
                dat.push_back(i);
            } else {
                dat.push_back(randint(0, alp_size - 1));
            }
        }
        shuffle(dat);
        for(int i = 0; i < n; i++) of << dat[i] << " \n"[i == n - 1];
        int q = 100000;
        of << q << "\n";
        while(q--) {
            int r, c;
            r = randint(1, n);
            c = randint(0, alp_size - 1);
            of << r << " " << c << "\n";
        }
    }
}
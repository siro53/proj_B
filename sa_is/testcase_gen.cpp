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

int main() {
    for(int t = 0; t < TESTCASE_NUM; t++) {
        int n = 900000;
        std::string s = "";
        for(int i = 0; i < n; i++) {
            char c = (char)('a' + randint(0, 25));
            s += c;
        }
        std::string filename = "test/random" + std::to_string(t + 1) + ".in";
        std::ofstream of(filename);
        of << s << "\n";
    }
}
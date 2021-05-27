#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "matrix.h"

const unsigned N = 4;

int main() {
    Matrix a(16, 16), b(16, 16);
    a.ReadFromFile("m1.txt");
    b.ReadFromFile("m1.txt");
    //Matrix c = Multiply(a, b, 8, N, N, N, N);
    //std::cout << c << std::endl;

    auto ranges = MakeRanges(8, 5);
    for (const auto& it : ranges) {
        std::cout << it.first << " " << it.second << std::endl;
    }

    return 0;
}

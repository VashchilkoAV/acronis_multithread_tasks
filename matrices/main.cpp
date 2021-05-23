#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "matrix.h"

const unsigned N = 8;

int main() {
    Matrix a(16, 16), b(16, 16);
    a.ReadFromFile("m1.txt");
    BlockedMatrix aBlocked(a, N, N);
    b.ReadFromBlockedMatrix(aBlocked);
    std::cout << b << std::endl;
    return 0;
}

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "matrix.h"

const unsigned N = 1;

int main() {
    Matrix a(3, 3), b(3, 3);
    a.ReadFromFile("m2.txt");
    b.ReadFromFile("m2.txt");
    Matrix c = Multiply(a, b, 1, N, N, N, N);
    std::cout << c << std::endl;
    return 0;
}

#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "matrix.h"

class Timer {
public:
    Timer(unsigned blockSize, unsigned numThreads, std::string fname) : _blockSize(blockSize), _numThreads(numThreads){
        std::ofstream _stream(fname);
        begin = std::chrono::steady_clock::now();
    }

    ~Timer() {
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        _stream << _numThreads << " " << _blockSize << " " << elapsed_ms.count() << " ms" << "\n";
        std::cout << _numThreads << " " << _blockSize << " " << elapsed_ms.count() << " ms" << "\n";
        _stream.flush();
        _stream.close();
    }

private:
    unsigned _blockSize, _numThreads;
    std::ofstream _stream;
    std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1, 1000000000>>>
            begin;


};

void Test(Matrix &a, Matrix& b) {
    std::ofstream result("result");
    std::vector<unsigned> blockShapes = {2, 4, 8, 16, 32, 64, 128};
    for (unsigned numThreads = 1; numThreads < 9; numThreads++) {
        for (const auto& size : blockShapes) {
            Timer timer(size, numThreads, "data.txt");
            Matrix c = Multiply(a, b, numThreads, size, size, size, size);
            result << c;
        }
    }
}

const unsigned N = 2048;

int main() {
    Matrix a(N, N), b(N, N);
    a.ReadFromFile("first");
    b.ReadFromFile("second");
    Test(a, b);


    //auto ranges = MakeRanges(8, 5);
    //for (const auto& it : ranges) {
    //    std::cout << it.first << " " << it.second << std::endl;
    //}

    return 0;
}

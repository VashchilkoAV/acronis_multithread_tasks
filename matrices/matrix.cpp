//
// Created by alexander on 5/24/21.
//

#include "matrix.h"

int Matrix::ReadFromFile(const std::string filename) {
    std::ifstream file(filename);
    std::string str;
    for (unsigned i = 0; i < _numRows; i++) {
        if (!std::getline(file, str)) {
            return 1;
        }
        std::stringstream ss(str);
        std::string num;
        for (int j = 0; j < _numColumns; j++) {
            if (!std::getline(ss, num, ' ')) {
                return 1;
            }
            _data[i][j] = std::stoi(num);
        }
    }
    return 0;
}

void Matrix::ReadFromBlockedMatrix (const BlockedMatrix& bm) {
    for (unsigned i = 0; i < _numRows; i++) {
        for (unsigned j = 0; j < _numColumns; j++) {
            _data[i][j] = bm.GetData()[i/bm.GetBlockNumRows()][j/bm.GetBlockNumColumns()].GetData()[(i % bm.GetBlockNumRows())*bm.GetBlockNumRows()+(j % bm.GetBlockNumColumns())];
        }
    }
}

std::ostream& operator<<(std::ostream &out, const Matrix & m) {
    for (unsigned i = 0; i < m._numRows; i++) {
        for (unsigned j = 0; j < m._numColumns; j++) {
            out << m._data[i][j] << " ";
        }
        out << "\n";
    }
    return out;
}

Matrix Multiply(const Matrix &left, const Matrix &right, unsigned int numThreads, unsigned leftBlockNumRows, unsigned leftBlockNumColumns, unsigned rightBlockNumRows, unsigned rightBlockNumColumns) {
    Matrix result(left.GetNumRows(), right.GetNumColumns());
    BlockedMatrix lBlocked(left, leftBlockNumRows, leftBlockNumColumns), rBlocked(right, rightBlockNumRows, rightBlockNumColumns);
    BlockedMatrix res = Multiply(lBlocked, rBlocked, numThreads);
    result.ReadFromBlockedMatrix(res);
    return result;
}

BlockedMatrix::BlockedMatrix(const Matrix& m, unsigned blockNumRows, unsigned  blockNumColumns):
_blockNumRows(blockNumRows),
_blockNumColumns(blockNumColumns),
_numColumns(m.GetNumColumns() % blockNumColumns == 0 ? m.GetNumColumns()/blockNumColumns : m.GetNumColumns()/blockNumColumns+1),
_numRows(m.GetNumRows() % blockNumRows == 0 ? m.GetNumRows()/_blockNumRows : m.GetNumRows()/_blockNumRows+1)
{
    _data = std::vector<std::vector<Block>>(_numRows, std::vector<Block>(_numColumns, Block(_blockNumRows, _blockNumColumns)));
    for (unsigned i = 0; i < m.GetNumRows(); i++) {
        for (unsigned j = 0; j < m.GetNumColumns(); j++) {
            _data[i/_blockNumRows][j/_blockNumColumns].GetData()[(i % _blockNumRows)*_blockNumRows+(j % _blockNumColumns)] = m.GetData().at(i).at(j);
        }
    }
}

Block Block::Transpose() {
    Block transposed(_numColumns, _numRows);
    for (unsigned i = 0; i < _numRows; i++) {
        for (unsigned j = 0; j < _numColumns; j++) {
            transposed.GetByIndex(j, i) = GetByIndex(i, j);
        }
    }
    return transposed;
}

Block operator+(const Block &left, const Block &right) {
    Block result(left._numRows,left._numColumns);
    for (unsigned i = 0; i < result._numRows; i++) {
        for (unsigned j = 0; j < result._numColumns; j++) {
            result.GetByIndex(i, j) = left.GetByIndex(i, j) + right.GetByIndex(i, j);
        }
    }
    return result;
}

Block &Block::operator+=(const Block &operand) {
    for (unsigned i = 0; i < _numRows; i++) {
        for (unsigned j = 0; j < _numColumns; j++) {
            GetByIndex(i, j) += operand.GetByIndex(i, j);
        }
    }
    return *this;
}

Block operator*(const Block &left, const Block &right) {
    Block result(left._numRows, left._numColumns);
    int tmp;
    for (unsigned i = 0; i < left._numRows; i++) {
        for (unsigned k = 0; k < left._numColumns; k++) {
            tmp = left.GetByIndex(i, k);
            for (unsigned j = 0; j < left._numColumns; j++) {
                result.GetByIndex(i, j) += tmp*right.GetByIndex(k, j);
            }
        }
    }
    return result;
}

std::vector<std::pair<unsigned, unsigned >> MakeRanges(unsigned numRows, unsigned numThreads) {
    std::vector<std::pair<unsigned, unsigned>> result(numThreads);
    unsigned delta = numRows % numThreads == 0 ? numRows / numThreads : numRows / numThreads + 1;
    result.back().first = (numThreads-1)*delta;
    result.back().second = numRows;
    unsigned prev = 0;
    for (unsigned i = 0; i < numThreads-1; i++) {
        result[i].first = prev;
        result[i].second = prev+delta;
        prev += delta;
    }
    return result;
}

BlockedMatrix Multiply(const BlockedMatrix &left, const BlockedMatrix &right, unsigned int numThreads) {
    BlockedMatrix result(left._numRows, right._numColumns, left._blockNumRows, right._blockNumColumns);
    auto job = [&](unsigned first, unsigned last) {
        Block tmp(left._numRows, left._numColumns);
        for (unsigned i = first; i < last; i++) {
            for (unsigned k = 0; k < left._numColumns; k++) {
                tmp = left._data[i][k];
                for (unsigned j = 0; j < left._numColumns; j++) {
                    result._data[i][j] += tmp*right._data[k][j];
                }
            }
        }
    };

    if (numThreads > left._numRows) {
        numThreads = left._numRows; //decreasing numThreads in order not to have threads with empty job
    }

    if (numThreads > 1) {
        std::vector<std::thread> workersPool(numThreads-1);
        auto ranges = MakeRanges(left._numRows, numThreads);

        for (unsigned i = 0; i < numThreads-1; i++) {
            workersPool[i] = std::thread(job, ranges[i].first, ranges[i].second);
        }

        job(ranges.back().first, ranges.back().second);

        for (unsigned i = 0; i < numThreads-1; i++) {
            if (workersPool[i].joinable()) {
                workersPool[i].join();
            }
        }
    } else {
        job(0, left._numRows);
    }



    //job(0, 16);
    //job(1, 2);

    return result;
}


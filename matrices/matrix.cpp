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

BlockedMatrix::BlockedMatrix(const Matrix& m, unsigned blockNumRows, unsigned  blockNumColumns): _blockNumRows(blockNumRows), _blockNumColumns(blockNumColumns),
                                                                                  _numColumns(m.GetNumColumns()/blockNumColumns), _numRows(m.GetNumRows()/_blockNumRows) {
    _data = std::vector<std::vector<Block>>(_numRows, std::vector<Block>(_numColumns, Block(_blockNumRows, _blockNumColumns)));
    for (unsigned i = 0; i < m.GetNumRows(); i++) {
        for (unsigned j = 0; j < m.GetNumColumns(); j++) {
            _data[i/_blockNumRows][j/_blockNumColumns].GetData()[(i % _blockNumRows)*_blockNumRows+(j % _blockNumColumns)] = m.GetData().at(i).at(j);
        }
    }
}

BlockedMatrix::Block BlockedMatrix::Block::Transpose() {
    Block transposed(_numColumns, _numRows);
    for (unsigned i = 0; i < _numRows; i++) {
        for (unsigned j = 0; j < _numColumns; j++) {
            transposed.GetByIndex(j, i) = GetByIndex(i, j);
        }
    }
    return transposed;
}
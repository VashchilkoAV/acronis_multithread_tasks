//
// Created by alexander on 5/24/21.
//

#ifndef MATRICES_MATRIX_H
#define MATRICES_MATRIX_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class Matrix {
public:
    Matrix(unsigned numRows, unsigned numColumns) : _numColumns(numColumns), _numRows(numRows) {
        _data = std::vector<std::vector<int>>(numRows, std::vector<int>(numColumns, 0));
    }

    std::vector<std::vector<int>> GetData() {
        return _data;
    }

    unsigned GetNumColumns() const{
        return _numColumns;
    }

    unsigned GetNumRows() const{
        return _numRows;
    }

    int ReadFromFile(const std::string filename) {
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

    void ReadFromBlockedMatrix (const BlockedMatrix& bm) {
        for (unsigned i = 0; i < _numRows; i++) {
            for (unsigned j = 0; j < _numColumns; j++) {
                _data[i][j] = bm.GetData()[i/bm.GetBlockNumRows][j/bm.GetBlockNumColumns()].GetData()[(i % bm.GetBlockNumRows())*bm.GetBlockNumRows()+(j % bm.GetBlockNumColumns())];
            }
        }
    }

    friend std::ostream& operator<< (std::ostream &out, const Matrix &m);

private:
    std::vector<std::vector<int>> _data;
    const unsigned _numColumns, _numRows;
};

std::ostream& operator<<(std::ostream &out, const Matrix & m) {
    for (unsigned i = 0; i < m._numRows; i++) {
        for (unsigned j = 0; j < m._numColumns; j++) {
            out << m._data[i][j] << " ";
        }
        out << "\n";
    }
    return out;
}

class BlockedMatrix {
public:
    struct Block;
    BlockedMatrix(const Matrix& m, unsigned blockNumRows, unsigned  blockNumColumns): _blockNumRows(blockNumRows), _blockNumColumns(blockNumColumns),
                                                                                      _numColumns(m.GetNumColumns()/blockNumColumns), _numRows(m.GetNumRows()/_blockNumRows) {
        _data = std::vector<std::vector<Block>>(_numRows, std::vector<Block>(_numColumns, Block(_blockNumRows, _blockNumColumns)));
        for (unsigned i = 0; i < m.GetNumRows(); i++) {
            for (unsigned j = 0; j < m.GetNumColumns(); j++) {
                _data[i/_blockNumRows][j/_blockNumColumns].GetData()[(i % _blockNumRows)*_blockNumRows+(j % _blockNumColumns)] = m.GetData().at(i).at(j);
            }
        }
    }

    unsigned GetBlockNumRows() const {
        return _blockNumRows;
    }

    unsigned GetBlockNumColumns() const {
        return _blockNumColumns;
    }

    std::vector<std::vector<Block>>& GetData() {
        return _data;
    }

private:
    struct Block{
    public:
        Block(unsigned numRows, unsigned numColumns) : _numRows(numRows), _numColumns(numColumns), _data(std::vector<int>(numRows*_numColumns, 0)){}

        int& GetByIndex(unsigned RowNum, unsigned ColumnNum) {
            return _data[RowNum*_numColumns+ColumnNum];
        }

        std::vector<int> & GetData() {
            return _data;
        }

        Block Transpose() {
            Block transposed(_numColumns, _numRows);
            for (unsigned i = 0; i < _numRows; i++) {
                for (unsigned j = 0; j < _numColumns; j++) {
                    transposed.GetByIndex(j, i) = GetByIndex(i, j);
                }
            }
            return transposed;
        }
    private:
        unsigned _numRows, _numColumns;
        std::vector<int> _data;
    };

    std::vector<std::vector<Block>> _data;
    const unsigned _blockNumColumns, _blockNumRows, _numColumns, _numRows;
};


#endif //MATRICES_MATRIX_H

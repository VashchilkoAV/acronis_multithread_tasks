//
// Created by alexander on 5/24/21.
//

#ifndef MATRICES_MATRIX_H
#define MATRICES_MATRIX_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class BlockedMatrix;

class Matrix {
public:
    Matrix(unsigned numRows, unsigned numColumns) : _numColumns(numColumns), _numRows(numRows) {
        _data = std::vector<std::vector<int>>(numRows, std::vector<int>(numColumns, 0));
    }

    const std::vector<std::vector<int>>& GetData() const{
        return _data;
    }

    std::vector<std::vector<int>>& GetData() {
        return _data;
    }

    unsigned GetNumColumns() const{
        return _numColumns;
    }

    unsigned GetNumRows() const{
        return _numRows;
    }

    int ReadFromFile(const std::string filename);

    void ReadFromBlockedMatrix (const BlockedMatrix& bm);

    friend std::ostream& operator<< (std::ostream &out, const Matrix &m);

private:
    std::vector<std::vector<int>> _data;
    const unsigned _numColumns, _numRows;
};

class BlockedMatrix {
private:
    struct Block{
    public:
        Block(unsigned numRows, unsigned numColumns) : _numRows(numRows), _numColumns(numColumns), _data(std::vector<int>(numRows*_numColumns, 0)){}

        int& GetByIndex(unsigned RowNum, unsigned ColumnNum) {
            return _data[RowNum*_numColumns+ColumnNum];
        }

        const int& GetByIndex(unsigned RowNum, unsigned ColumnNum) const{
            return _data[RowNum*_numColumns+ColumnNum];
        }

        std::vector<int> & GetData() {
            return _data;
        }

        const std::vector<int> & GetData() const {
            return _data;
        }

        Block Transpose();
    private:
        unsigned _numRows, _numColumns;
        std::vector<int> _data;
    };

    std::vector<std::vector<Block>> _data;
    const unsigned _blockNumColumns, _blockNumRows, _numColumns, _numRows;
public:
    BlockedMatrix(const Matrix& m, unsigned blockNumRows, unsigned  blockNumColumns);

    unsigned GetBlockNumRows() const {
        return _blockNumRows;
    }

    unsigned GetBlockNumColumns() const {
        return _blockNumColumns;
    }

    const std::vector<std::vector<Block>>& GetData() const{
        return _data;
    }

    std::vector<std::vector<Block>>& GetData() {
        return _data;
    }
};


#endif //MATRICES_MATRIX_H

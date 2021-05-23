#include <iostream>
#include <thread>
#include <vector>

class Matrix {
    Matrix(unsigned numRows, unsigned numColumns) : _numColumns(numColumns), _numRows(_numRows), {
        _data = std::vector<std::vector<int>>(_numRows, std::vector<int>(numColumns, 0));
    }

    const & std::vector<std::vector<int>> GetData() {
        return _data;
    }

    const unsigned GetNumColumns() {
        return _numColumns;
    }

    const unsigned GetNumRows() {
        return _numRows;
    }

private:
    std::vector<std::vector<int>> _data;
    const unsigned _numColumns, _numRows;
};

class BlockedMatrix {
    BlockedMatrix(const Matrix& m, unsigned blockNumRows, unsigned  blockNumColumns): _blockNumRows(blockNumRows), _blockNumColumns(blockNumColumns),
    _numColumns(m.GetNumColumns()/blockNumColumns), _numRows(m.GetNumRows()/_blockNumRows),
    _data(std::vector<std::vector<Block>>(_numRows, std::vector<Block>(_numColumns, Block()))) {
        for (unsigned i = 0; i < m.GetNumRows(); i++) {
            for (unsigned j = 0; j < m.GetNumColumns(); j++) {
                _data[i/_blockNumRows][j/_blockNumColumns].GetData()[]
            }
        }
    }

    friend BlockedMatrix Multiple(BlockedMatrix left, BlockedMatrix right, unsigned numThreads);

private:
    struct Block{
    public:
        Block(unsigned numRows, unsigned numColumns) : _numRows(numRows), _numColumns(numColumns), _data(std::vector<int>(numRows*_numColumns, 0)){}

        int& GetByIndex(unsigned RowNum, unsigned ColumnNum) {
            return _data[RowNum*_numColumns+ColumnNum];
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

BlockedMatrix Multiple(BlockedMatrix left, BlockedMatrix right, unsigned numThreads) {

}

int main() {
    Matrix a(1, 2);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

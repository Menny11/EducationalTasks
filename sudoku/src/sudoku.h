/*
 * sudoku.h
 *
 * Sudoku solver header
 * created by Mykola Fedorenko stnikolay@ukr.net
 * 
 */

#ifndef SUDOKU_H
#define SUDOKU_H

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <exception>

typedef int Cell;
typedef std::vector<Cell> Row;
class BitMask;
class Table
{
public:
	Table() = default;
	Table(std::vector<Row>& inMatr): 
        mTable{inMatr},
        mSolvedCells{0},
        mSize{int(inMatr.size())},
        mBox{int(std::sqrt(mSize))}{}
    std::vector<Row> sudoku();
private:
    std::vector<Row> mTable;
    int mSolvedCells=0, mSize=0, mBox=0;
    bool isBacktrackingEnabled=false;
    struct LowestCountElement{
    public:
        int iRow=0, jCol=0;
    } mLowestCount;
    void initTable(std::vector<BitMask>& hlMask, 
                   std::vector<BitMask>& vlMask, 
                   std::vector<BitMask>& box);
    bool searchTable(std::vector<Row>& tbl, 
                    std::vector<BitMask>& hlMasks, 
                    std::vector<BitMask>& vlMasks, 
                    std::vector<BitMask>& boxMasks);
    bool searchHidden(std::vector<Row>& curTable, 
                     std::vector<std::vector<BitMask>>& tbl, 
                     std::vector<BitMask>& hlMasks, 
                     std::vector<BitMask>& vlMasks, 
                     std::vector<BitMask>& boxMasks);
    void updateCell(std::vector<Row>& tbl, int x, int y, int num, 
                    std::vector<BitMask>& hlMasks,
                    std::vector<BitMask>& vlMasks, 
                    std::vector<BitMask>& boxMasks);
    bool sudokuBacktrack(std::vector<Row> curTable, 
                         std::vector<BitMask> hlMasks, 
                         std::vector<BitMask> vlMasks, 
                         std::vector<BitMask> boxMasks);
};

class BadInitTableSizeException: public std::exception {
public:
    virtual const char* what() const throw(){
        return "Improper size of a Table!";
    }
};
    
class ImproperAssignmentException: public std::exception {
public:
    ImproperAssignmentException(int x, int y)
        :mX{x}, mY{y}{}
    virtual const char* what() const throw(){
        std::string errMessage=std::string("Impossible number for Cell (")
            +std::to_string(mX)+", "+ std::to_string(mY)+");";
        return errMessage.c_str();
    }
private:
    int mX=0, mY=0;
};

class BitMask
{
    using int32uns = unsigned long;
    friend BitMask operator | (const BitMask& x, const BitMask& y);
    friend BitMask operator & (const BitMask& x, const BitMask& y);
    friend BitMask operator ^ (const BitMask& x, const BitMask& y);
public:
    BitMask():mVal(1, ~0ul){}
    explicit BitMask(int bitLen):mVal(bitLen/32+1, ~0ul){
        mVal.back()&=(1ull<<(bitLen%32))-1;}
    bool operator==(const BitMask& rhs){return (mVal==rhs.mVal);}
    bool operator!=(const BitMask& rhs){return (mVal!=rhs.mVal);}
    int getIndex(); // returns index of a highest bit set
    BitMask& unSet(int ind); //unsets bit with index 'ind'
    int getBit(int ind); // returns bit with index 'ind'
    int getBitsNum(); //returns a number of nonzero bits
    void clear(){for (auto& el:mVal) el=0;}; // clear all bits
private:
    std::vector<int32uns> mVal;
};


#endif // SUDOKU_H

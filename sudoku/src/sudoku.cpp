/* Sudoku solver
* author Mykola Fedorenko stnikolay@ukr.net
* this solver uses backtracking algorithm 
* with search for single elements and hidden single
*/
#include "sudoku.h"

int BitMask::getBit(int index){
    --index;
    return ((mVal[index/32]&(1ull<<(index%32)))&&1);
}

int BitMask::getIndex(){
    int index=0;
    unsigned long long xNum;
    unsigned long long binSearchConst[]=
        {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    int shifts[]={1,2,4,8,16};
    for (auto& el:mVal){
        if (el){
            xNum=el;
            for (int i=4; i>=0; --i){
                if (xNum & binSearchConst[i]){
                    xNum>>=shifts[i];
                    index|=shifts[i];
                }
            }
        }
    }
    ++index;
    return index;
}

inline BitMask& BitMask::unSet(int index){
    --index;
    mVal[index/32]&=~(1ull<<(index%32));
    return *this;
} 

int BitMask::getBitsNum(){
    int counter=0, locCounter;
    unsigned int num;
    for (std::size_t i=0; i<mVal.size(); ++i){
        num=mVal[i];
        if (num==0) break;
        for (locCounter=0; num; ++locCounter){
            num&=num-1;
        }
        counter+=locCounter;
    }
    return counter;
}

BitMask operator | (const BitMask& x, const BitMask& y){
    BitMask res(x);
    for (std::size_t i=0; i<x.mVal.size(); ++i)
        res.mVal[i]|=y.mVal[i];
    return res;
}
BitMask operator & (const BitMask& x, const BitMask& y){
    BitMask res(x);
    for (std::size_t i=0; i<x.mVal.size(); ++i)
        res.mVal[i]&=y.mVal[i];
    return res;
}
BitMask operator ^ (const BitMask& x, const BitMask& y){
    BitMask res(x);
    for (std::size_t i=0; i<x.mVal.size(); ++i)
        res.mVal[i]^=y.mVal[i];
    return res;
}

void Table::initTable(std::vector<BitMask>& hlMasks, 
                      std::vector<BitMask>& vlMasks, 
                      std::vector<BitMask>& boxMasks){
    //scan lines for initial numbers
    if (mBox*mBox!=mSize) 
        throw BadInitTableSizeException();
    for (int i=0, tmp=0; i<mSize; ++i){
        for (int j=0; j<mSize; ++j){
            tmp=mTable[i][j];
            if (tmp){
                if (!hlMasks[i].getBit(tmp)||!vlMasks[j].getBit(tmp)||
                    !boxMasks[i-i%mBox+j/mBox].getBit(tmp))
                    throw ImproperAssignmentException(i, j);
                hlMasks[i].unSet(tmp);
                vlMasks[j].unSet(tmp);
                boxMasks[i-i%mBox+j/mBox].unSet(tmp);
                ++mSolvedCells;
            }
        }
    }
}

inline 
void Table::updateCell(std::vector<Row>& table, int x, int y, int num, 
                       std::vector<BitMask>& hlMasks,
                       std::vector<BitMask>& vlMasks, 
                       std::vector<BitMask>& boxMasks){
    table[x][y]=num;
    hlMasks[x].unSet(num);
    vlMasks[y].unSet(num);
    boxMasks[x-x%mBox+y/mBox].unSet(num);
    ++mSolvedCells;
}

bool Table::searchTable(std::vector<Row>& curTable, 
                       std::vector<BitMask>& hlMasks, 
                       std::vector<BitMask>& vlMasks, 
                       std::vector<BitMask>& boxMasks){
    static std::vector<std::vector<BitMask>> 
        tbl(mSize, std::vector<BitMask>(mSize, BitMask(mSize)));
    //set possible numbers for cells and search for single
    BitMask curMask(mSize);
    int possibleValues=0, minAll=mSize;
    for (int i=0; i<mSize; ++i){
        for (int j=0; j<mSize; ++j){
            if (curTable[i][j]==0){
                curMask=hlMasks[i] & vlMasks[j] 
                    & boxMasks[i-i%mBox+j/mBox];
                possibleValues=curMask.getBitsNum();
                if (possibleValues>1){
                    if (minAll>possibleValues){
                        minAll=possibleValues;
                        mLowestCount.iRow=i;
                        mLowestCount.jCol=j;
                    }
                }else if (possibleValues==1){
                    updateCell(curTable, i, j, curMask.getIndex(), 
                               hlMasks, vlMasks, boxMasks);
                    continue;
                } else if (possibleValues==0){
                    if (isBacktrackingEnabled)
                        return false;
                    else
                        throw ImproperAssignmentException(i, j);
                }
                tbl[i][j]=curMask;
            }
        }
    }
    //search for hidden elems
    searchHidden(curTable, tbl, hlMasks, vlMasks, boxMasks);
    return true;
}
inline 
bool Table::searchHidden(std::vector<Row>& curTable, 
                        std::vector<std::vector<BitMask>>& tbl, 
                        std::vector<BitMask>& hlMasks, 
                        std::vector<BitMask>& vlMasks, 
                        std::vector<BitMask>& boxMasks){    
    // search for hidden single elems
    //in rows
    BitMask curMask(mSize), sumMask(mSize), andMask(mSize);
    BitMask emptyMask(mSize), tmpMask(mSize);
    emptyMask.clear();
    for (int i=0; i<mSize; ++i){
        sumMask=hlMasks[i];
        curMask=emptyMask;
        for (int j=0; j<mSize; ++j){
            if (curTable[i][j]==0){
                andMask=curMask & tbl[i][j];
                curMask=(curMask ^ tbl[i][j]) & sumMask;
                sumMask=sumMask ^ andMask;
            }
        }
        if (curMask.getBitsNum()){
            for (int k=0; k<mSize; ++k){
                if (curTable[i][k]==0){
                    tmpMask=curMask & tbl[i][k];
                    if (tmpMask!=emptyMask){
                        updateCell(curTable, i, k, tmpMask.getIndex(), 
                                   hlMasks, vlMasks, boxMasks);
                    }
                }
            }
        }
    }
    //in cols
    for (int j=0; j<mSize; ++j){
        sumMask=vlMasks[j];
        curMask=emptyMask;
        for (int i=0; i<mSize; ++i){
            if (curTable[i][j]==0){
                andMask=curMask & tbl[i][j];
                curMask=(curMask ^ tbl[i][j]) & sumMask;
                sumMask=sumMask ^ andMask;
            }
        }
        if (curMask.getBitsNum()){
            for (int k=0; k<mSize; ++k){
                if (curTable[k][j]==0){
                    tmpMask=curMask & tbl[k][j];
                    if (tmpMask!=emptyMask){
                        updateCell(curTable, k, j, tmpMask.getIndex(),
                                   hlMasks, vlMasks, boxMasks);
                    }
                }
            }
        }
    }
    //in boxes
    for (int boxInd=0, iBeg, iEnd, jBeg, jEnd; 
                    boxInd<mSize; ++boxInd){
        iBeg=(boxInd/mBox)*mBox;
        iEnd=iBeg+mBox;
        jBeg=(boxInd%mBox)*mBox;
        jEnd=jBeg+mBox;
        sumMask=boxMasks[boxInd];
        curMask=emptyMask;
        for (int i=iBeg; i<iEnd ; ++i){
            for (int j=jBeg; j<jEnd; ++j){
                if (curTable[i][j]==0){
                    andMask=curMask & tbl[i][j];
                    curMask=(curMask ^ tbl[i][j]) & sumMask;
                    sumMask=sumMask ^ andMask;
                }
            }
        }
        if (curMask.getBitsNum()){
            for(int i=iBeg; i<iEnd; ++i){
                for (int j=jBeg; j<jEnd; ++j){
                    if (curTable[i][j]==0){
                        tmpMask=curMask & tbl[i][j];
                        if (tmpMask!=emptyMask){
                            updateCell(curTable, i, j, tmpMask.getIndex(),
                                       hlMasks, vlMasks, boxMasks);
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool Table::sudokuBacktrack(std::vector<Row> curTable, 
                            std::vector<BitMask> hlMasks, 
                            std::vector<BitMask> vlMasks, 
                            std::vector<BitMask> boxMasks){
    int prevSolvedNum=-1;
    while (prevSolvedNum!=mSolvedCells){
        prevSolvedNum=mSolvedCells;
        if (!searchTable(curTable, hlMasks, vlMasks, boxMasks)){
            return false;
        }
    }
    if (mSolvedCells==mSize*mSize){
        mTable=curTable;
        return true;
    }
    int tRow=mLowestCount.iRow, tCol=mLowestCount.jCol;
    int boxInd=tRow-tRow%mBox+tCol/mBox;
    BitMask optElemMask=hlMasks[tRow] & vlMasks[tCol] & boxMasks[boxInd];
    int bitsNum=optElemMask.getBitsNum();
    BitMask prevHlM, prevVlM, prevBox;
    for (int k=0, ind, prevSolved; k<bitsNum; ++k){
        //attempt to guess value
        ind=optElemMask.getIndex();
        optElemMask.unSet(ind);
        prevSolved=mSolvedCells;
        prevHlM=hlMasks[tRow];
        prevVlM=vlMasks[tCol];
        prevBox=boxMasks[boxInd];
        updateCell(curTable, tRow, tCol, ind,
                  hlMasks, vlMasks, boxMasks);
        if (sudokuBacktrack(curTable, hlMasks, vlMasks, boxMasks)){
            return true;
        }
        //bad guess return to prev values
        mSolvedCells=prevSolved;
        hlMasks[tRow]=prevHlM;
        vlMasks[tCol]=prevVlM;
        boxMasks[boxInd]=prevBox;
    }
    return false;
}

std::vector<Row> Table::sudoku() {
    std::vector<BitMask> hLineMasks(mSize, BitMask(mSize));
    std::vector<BitMask> vLineMasks(mSize, BitMask(mSize));
    std::vector<BitMask> boxMasks(mSize, BitMask(mSize));
    try{
    initTable(hLineMasks, vLineMasks, boxMasks);
    }
    catch(ImproperAssignmentException& rAs){
        std::cout<<"Error! : "<<rAs.what()<<std::endl;
        return mTable;
    }
    catch (BadInitTableSizeException& bInTab){
        std::cout<<"Error! : "<<bInTab.what()<<std::endl;
        return mTable;
    }
    //finds elements in table without any assumptions 
    int prevSolvedNum=-1;
    while (prevSolvedNum!=mSolvedCells){
        prevSolvedNum=mSolvedCells;
        searchTable(mTable, hLineMasks, vLineMasks, boxMasks);
    }
    if (mSolvedCells==mSize*mSize)
        return mTable;
    //backtrack rest of the elements if there are unsolved
    isBacktrackingEnabled=true;
    if(!sudokuBacktrack(mTable, hLineMasks, vLineMasks, boxMasks)){
        std::cout<<"Unable to solve this sudoku!"<<std::endl;
    }
    return mTable;
}


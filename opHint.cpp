#include "sudoku.hpp"

// 候補から (row, col, num) 関連を追加
void Sudoku::addCandidateNumbers(int row, int col, int num) {
    Mass[row * 9 + col] = 9;
    
    // 対象のブロックの左上
    int row_block = (row / 3) * 3;
    int col_block = (col / 3) * 3;

    // 使用している数から削除
    ColUsedNumber[col * 9 + num] = false;
    RowUsedNumber[row * 9 + num] = false;
    BlockUsedNumber[(row_block + (col / 3)) * 9 + num] = false;

    // 候補に追加しなおす
    int idiv3, imod3;
    for (int i = 0; i < 9; i++) {
        idiv3 = i / 3;
        imod3 = i % 3;

        if (!(RowUsedNumber[i * 9 + num] || BlockUsedNumber[(3 * idiv3 + col / 3) * 9 + num]))
            CandidateNumber[i * 81 + col * 9 + num] = true;

        if (!(ColUsedNumber[i * 9 + num] || BlockUsedNumber[(row_block + idiv3) * 9 + num]))
            CandidateNumber[row * 81 + i * 9 + num] = true;

        if (!(RowUsedNumber[(row_block + idiv3) * 9 + num] || ColUsedNumber[(col_block + imod3) * 9 + num]))
            CandidateNumber[(row_block + idiv3) * 81 + (col_block + imod3) * 9 + num] = true;
    }
}

// 候補から (row, col, num) 関連を削除
void Sudoku::discardCandidateNumbers(int row, int col, int num) {
    Mass[row * 9 + col] = num;

    // 対象のブロックの左上
    int row_block = (row / 3) * 3;
    int col_block = (col / 3) * 3;

    // 使用している数に追加
    ColUsedNumber[col * 9 + num] = true;
    RowUsedNumber[row * 9 + num] = true;
    BlockUsedNumber[(row_block + (col / 3)) * 9 + num] = true;

    // 候補から削除
    for (int i = 0; i < 9; i++) {
        CandidateNumber[i * 81 + col * 9 + num] = false;
        CandidateNumber[row * 81 + i * 9 + num] = false;
        CandidateNumber[(row_block + (i / 3)) * 81 + (col_block + (i % 3)) * 9 + num] = false;
    }
}

// ヒントに追加
void Sudoku::addHint(int row, int col, int num) {
    int mass = row * 9 + col;
    Mass[mass] = num;
    Hints[mass] = num;
    discardCandidateNumbers(row, col, num);
    
    auto iter = changePair.begin();
    while (iter != changePair.end()) {
        if (mass == std::get<0>(*iter) || mass == std::get<1>(*iter))
            iter = changePair.erase(iter);
        else
            iter++;
    }
}

// ヒントから削除
void Sudoku::deleteHint(int row, int col, int num) {
    int mass = row * 9 + col;
    Mass[mass] = 9;
    if (Hints.find(mass) == Hints.end()) {
        printf("there is not (row, col, num) in Hints\n");
        exit(1);
    }
    Hints.erase(mass);
    addCandidateNumbers(row, col, num);
    
    // massと被る、かつ他ヒントと被らない2マスをchangepairに追加する
    int upper_row = row / 3 * 3;
    int left_col = col / 3 * 3;
    for (int offset = 0; offset < 9; offset++){
        int mass2 = (upper_row + offset / 3) * 9 + (left_col + offset % 3);
        if (Hints.find(mass2) != Hints.end() && mass != mass2){
            changePair.push_back(std::make_tuple(mass, mass2));
        }
    }
}

void Sudoku::init() {
    for (int i = 0; i < 81; i++) {
        Mass[i] = 9;
        RowUsedNumber[i] = false;
        ColUsedNumber[i] = false;
        BlockUsedNumber[i] = false;
    }

    for (int i = 0; i < 729; i++) {
        CandidateNumber[i] = true;
        Skip[i] = false;
    }

    CandidateHints.clear();
    Hints.clear();
    changePair.clear();

    ConvergeCount17 = 100000000;

    for (int block = 0; block < 9; block++) {
        for (int offset1 = 0; offset1 < 9; offset1++) {
            int row1 = (block / 3) * 3 + (offset1 / 3);
            int col1 = (block % 3) * 3 + (offset1 % 3);
            for (int offset2 = offset1 + 1; offset2 < 9; offset2++) {
                int row2 = (block / 3) * 3 + (offset2 / 3);
                int col2 = (block % 3) * 3 + (offset2 % 3);
                changePair.push_back(std::make_pair(row1 * 9 + col1, row2 * 9 + col2)); 
            }
        }
    }
    
    bfrId = "";
}

void Sudoku::firstHint() {
    std::random_device rand;
    int row = rand() % 9;
    int col = rand() % 9;
    int num = rand() % 9;
    addHint(row, col, num);
}

void Sudoku::createCandidateHints() {
    CandidateHints.clear();
    for (int mass1 = 0; mass1 < 81; mass1++) {
        if (Mass[mass1] != 9) continue;
        for (int n = 0; n < 9; n++) {
            int mass2 = mass1 * 9 + n;
            if (!CandidateNumber[mass2] || Skip[mass2]) continue;

            if (Hints.size() >= 13) {
                discardCandidateNumbers(mass1 / 9, mass1 % 9, n);
                if (backtrack()) {
                    CandidateHints[mass2] = 0;
                } else
                    Skip[mass2] = true;
                    
                addCandidateNumbers(mass1 / 9, mass1 % 9, n);
            } else {
                // printf("mass %d\n", mass2);
                CandidateHints[mass2] = 0;
            }
        }
    }
}
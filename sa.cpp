#include "sudoku.hpp"

int start_pat[81];
int pat[81];
int col_value[81], row_value[81];

inline float defineBeta(int size) {
    return 5.5;
    // return size < 11 ? 4.0 : 3.5;
}

// int saInitPatMass(int row, bool *used) {
//     int res = -1;
//     int count = 10;
    
//     // rowで一番候補が少ないかつMassが埋まっていないマスを調べる
//     for (int col = 0; col < 9; col++) {
//         if (start_pat[row * 9 + col] >= 0 && start_pat[row * 9 + col] < 9) continue;
        
//         // mass = (row, col)の候補数をカウント
//         int c = 0;
//         int n = 0;
//         for (int num = 0; num < 9; num++) {
//             if (!used[num]) {
//                 c += 1;
//                 n = num;
//             }
//         }

//         // 更新
//         if (count > c) {
//             res = col * 9 + n;
//             count = c;
//         }
//     }
//     return res;
// }

void Sudoku::saInitPat() {
    // 最初は9をpatに設定する
    for (int mass = 0; mass < 81; mass++) {
        start_pat[mass] = 9;
    }

    for (auto hint : Hints) {
        start_pat[hint.first] = hint.second;
    }

    // ヒント以外のマスに同ブロックで0~8が揃うように数を設定する
    for (int block = 0; block < 9; block++) {
        // ヒントで使う数字をtrueにする
        bool use_nums[9] = { false, false, false, false, false, false, false, false, false };
        for (int offset = 0; offset < 9; offset++) {
            int row = (block / 3) * 3 + (offset / 3);
            int col = (block % 3) * 3 + (offset % 3);
            int num = start_pat[row * 9 + col];
            if (num != 9) {
                use_nums[num] = true;
            }
        }

        // ヒントで使用した数字を集計したので、ヒント以外のマスに同ブロックで
        // 0 ~ 8が揃うように初期盤面を設定する
        for (int offset = 0; offset < 9; offset++) {
            int row = (block / 3) * 3 + (offset / 3);
            int col = (block % 3) * 3 + (offset % 3);
            int mass = row * 9 + col;
            if (start_pat[mass] != 9) continue;

            for (int num = 0; num < 9; num++) {
                if (use_nums[num]) continue;

                use_nums[num] = true;
                start_pat[mass] = num;
                break;
            }
        }
    }
}

inline void Sudoku::keepRecord() {
    for (int mass = 0; mass < 81; mass++) {
        int candidate = mass * 9 + pat[mass];
        if (CandidateHints.find(candidate) != CandidateHints.end()) {
            CandidateHints[candidate] += 1;
        } else if (Hints.find(mass) != Hints.end()) 
            continue;
        else {
            printf("keepRecord error : %d there is not candidate in CandidateHints\n", candidate);
            exit(1);
        }
    }
}

// 交換可能な2マスの選択
inline std::tuple<int, int> Sudoku::massChoice() {
    int r = rand() % changePair.size();
    return changePair.at(r);
}

// 盤面の評価値を計算
float calc_value() {
    int value = 0;

    for (int mass = 0; mass < 81; mass++) {
        col_value[mass] = 0; row_value[mass] = 0;
    }

    // 使っている数字分評価値に足す
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            int mass = row * 9 + col;
            col_value[col * 9 + pat[mass]]++;
            row_value[row * 9 + pat[mass]]++;
        }
    }

    // 使っていない数字を評価値に足す
    for (int i = 0; i < 81; i++) {
        if (col_value[i] == 0) value++;
        if (row_value[i] == 0) value++;
    }

    return value;
}
// int value() {
//     int v = 0; // 評価値

//     for (int i = 0; i < 81; i++) { col_value[i] = 0; block_value[i] = 0; }

//     // 使っていない数字分足す
//     for (int i = 0; i < 9; i++) {
//         int block_i = i / 3;

//         for (int j = 0; j < 9; j++) {
//             int block = block_i * 3 + j / 3;
//             col_value[j * 9 + pat[i * 9 + j]]++;
//             block_value[block * 9 + pat[i * 9 + j]]++;
//         }
//     }

//     for (int i = 0; i < 81; i++) {
//         if (!col_value[i]) v++;
//         if (!block_value[i]) v++;
//     }
//     return v;
// }

inline int delta_value(int row1, int col1, int row2, int col2) {
    // 列 * 9 + pat[行 * 9 + 列] を計算して、col_valueの変化するindexを得る
    int bfr_m1_col_ind = col1 * 9 + pat[row2 * 9 + col2];
    int bfr_m2_col_ind = col2 * 9 + pat[row1 * 9 + col1];
    int aft_m1_col_ind = col1 * 9 + pat[row1 * 9 + col1];
    int aft_m2_col_ind = col2 * 9 + pat[row2 * 9 + col2];

    // 行 * 9 + pat[行 * 9 + 列] を計算して、row_valueの変化するindexを得
    int bfr_m1_row_ind = row1 * 9 + pat[row2 * 9 + col2];
    int bfr_m2_row_ind = row2 * 9 + pat[row1 * 9 + col1];
    int aft_m1_row_ind = row1 * 9 + pat[row1 * 9 + col1];
    int aft_m2_row_ind = row2 * 9 + pat[row2 * 9 + col2];

    int aft_m1_row_val = row_value[aft_m1_row_ind];
    int aft_m2_row_val = row_value[aft_m2_row_ind];
    int aft_m1_col_val = col_value[aft_m1_col_ind];
    int aft_m2_col_val = col_value[aft_m2_col_ind];

    col_value[bfr_m1_col_ind]--;
    col_value[bfr_m2_col_ind]--;
    col_value[aft_m1_col_ind]++;
    col_value[aft_m2_col_ind]++;

    row_value[bfr_m1_row_ind]--;
    row_value[bfr_m2_row_ind]--;
    row_value[aft_m1_row_ind]++;
    row_value[aft_m2_row_ind]++;

    int delta = (!col_value[bfr_m1_col_ind] - !aft_m1_col_val) +
                (!col_value[bfr_m2_col_ind] - !aft_m2_col_val) +
                (!row_value[bfr_m1_row_ind] - !aft_m1_row_val) +
                (!row_value[bfr_m2_row_ind] - !aft_m2_row_val);
    // printf("delta %d\n", delta);
    return delta;
}

int Sudoku::refine() {
    struct timeval time;
    gettimeofday(&time, nullptr);
    unsigned int time_ = time.tv_sec + time.tv_usec;
    srand(time_);
    int v1, v2;
    std::tuple<int, int> mass;
    int mass1, mass2, temp;

    const float START_BETA = 0;
    const float END_BETA = defineBeta(Hints.size());
    const int LOOP_LIMIT = 50000;
    const int CYCLE = 50;
    const float DELTA_BETA = (END_BETA - START_BETA) / (float)CYCLE; // SAのβの加算値

    v1 = calc_value();

    float beta = START_BETA;
    for (int i = 0; i < LOOP_LIMIT; i++) {
        if (i > 100 && v1 == 0) {
            keepRecord();
            return 1;
        }

        mass = massChoice();
        mass1 = std::get<0>(mass);
        mass2 = std::get<1>(mass);

        // 交換と評価
        std::swap(pat[mass1], pat[mass2]);
        v2 = v1 + delta_value(mass1 / 9, mass1 % 9, mass2 / 9, mass2 % 9);

        // 交換前の方が評価が低いなら交換採用
        if (v1 < v2) {
            float r = (float)(rand() % RAND_MAX) / (float)(RAND_MAX);
            if (1 < r * exp((v2 - v1) * beta)) {
                std::swap(pat[mass1], pat[mass2]);
                delta_value(mass1 / 9, mass1 % 9, mass2 / 9, mass2 % 9);
            } else 
                v1 = v2;
        } else
            v1 = v2;
        
        // ループ回数がある定数倍ならβを更新する
        if (i > 0 && i % (LOOP_LIMIT / CYCLE) == 0) {
            beta += DELTA_BETA;
        }
    }
    return 0;
}

int Sudoku::doingSATimes(int count) {
    int converge_count = 0;

    createCandidateHints();

    saInitPat();

    for (int i = 0; i < count; i++) {
        memcpy(pat, start_pat, sizeof(int)*81);
        converge_count += refine();
    }
    
    return converge_count;
}
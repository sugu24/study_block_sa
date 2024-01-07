#include "sudoku.hpp"

// 現在一番ヒント候補の候補が少ないマスを返す（埋まる:-1,行き止まり:-2）
int Sudoku::createLeastMass() {
    int mass = -1;
    int least_count = 10;
    int count;

    for (int i = 0; i < 81; i++) {
        if (Mass[i] != 9) continue;
        count = 0;
        for (int j = 0; j < 9; j++)
            if (CandidateNumber[i * 9 + j] && !Skip[i * 9 + j]) 
                count += 1;
        
        // 埋まってないマスの候補が0個の場合
        if (count == 0) return -2;

        // 候補の数が少ない
        if (least_count > count) {
            least_count = count;
            mass = i;
        }
    }
    return mass;
}

// backtrack
bool Sudoku::backtrack() {
    int mass = createLeastMass(); // 対象マス
    bool converge;

    if (mass == -2) return false;
    if (mass == -1) return true;

    // hintsで与えられているマスか
    if (Mass[mass] != 9)
        return backtrack();
    else {
        for (int num = 0; num < 9; num++) {
            // マス的に入らないかバックトラックで入らないことが確定している場合continue
            if (!CandidateNumber[mass * 9 + num] || Skip[mass * 9 + num]) continue;

            // いったん候補から消す
            discardCandidateNumbers(mass / 9, mass % 9, num);

            // backtrack
            converge = backtrack();

            // 候補を戻す
            addCandidateNumbers(mass / 9, mass % 9, num);

            if (converge) return converge;
        }
    }
    return false;
}

// -------------------- backtrack all -------------------- //
void Sudoku::recordAppearance() {
    for (int i = 0; i < 81; i++)
        CandidateHints[i * 9 + Mass[i]] += 1;
}

// backtrackAll
void Sudoku::backtrackAll(BACKTRACKRECORD record, int conv_limit) {
    if (Count >= conv_limit) return;
    int mass = createLeastMass(); // 対象マス

    if (mass == -2) return;
    if (mass == -1) { 
        Count += 1;
        if (record) recordAppearance(); 
        return; 
    }

    // hintsで与えられているマスか
    if (Mass[mass] != 9)
        backtrackAll(record, conv_limit);
    else {
        int mass2;
        for (int num = 0; num < 9; num++) {
            // マス的に入らないかバックトラックで入らないことが確定している場合continue
            if (!CandidateNumber[mass * 9 + num] || Skip[mass * 9 + num]) continue;

            // いったん候補から消す
            discardCandidateNumbers(mass / 9, mass % 9, num);

            // backtrack
            backtrackAll(record, conv_limit);

            // 候補を戻す
            addCandidateNumbers(mass / 9, mass % 9, num);
        }
    }
    return;
}

int Sudoku::doBacktrackAll(bool init_cand, BACKTRACKRECORD record, int conv_limit) {
    Count = 0;
    if (init_cand) createCandidateHints();
    backtrackAll(record, conv_limit);
    return Count;
}

void Sudoku::doBacktrack15_17() {
    int skip1[729], skip2[729], skip3[729];
    int hint15, hint16, hint17;
    int least_conv = 100000;

    createCandidateHints();

    memcpy(skip1, Skip, 729);
    // 1つ目
    for (int mass1 = 0; mass1 < 79; mass1++) {
        if (Mass[mass1] != 9) continue;
        for (int num1 = 0; num1 < 9; num1++) {
            if (Skip[mass1 * 9 + num1] || !CandidateNumber[mass1 * 9 + num1]) continue;
            // printf("%d conv %d\n", mass1 * 9 + num1, least_conv);
            discardCandidateNumbers(mass1 / 9, mass1 % 9, num1);
            memcpy(Skip, skip1, 729);
            createCandidateHints();
            memcpy(skip2, Skip, 729);
            // 2つ目決める
            for (int mass2 = mass1+1; mass2 < 81; mass2++) {
                if (Mass[mass2] != 9) continue;
                for (int num2 = 0; num2 < 9; num2++) {
                    if (Skip[mass2 * 9 + num2] || !CandidateNumber[mass2 * 9 + num2]) continue;
                    discardCandidateNumbers(mass2 / 9, mass2 % 9, num2);
                    memcpy(Skip, skip2, 729);
                    createCandidateHints();
                    memcpy(skip3, Skip, 729);
                    for (int mass3 = mass2+1; mass3 < 81; mass3++) {
                        if (Mass[mass3] != 9) continue;
                        for (int num3 = 0; num3 < 9; num3++) {
                            if (Skip[mass3 * 9 + num3] || !CandidateNumber[mass3 * 9 + num3]) continue;
                            discardCandidateNumbers(mass3 / 9, mass3 % 9, num3);
                            // memcpy(Skip, skip3, 729);
                            doBacktrackAll(false, ZERO, least_conv);
                            if (Count > 0 && Count < least_conv) {
                                hint15 = mass1 * 9 + num1;
                                hint16 = mass2 * 9 + num2;
                                hint17 = mass3 * 9 + num3;
                                least_conv = Count;
                                // printf("conv %d (%d %d %d)\n", least_conv, hint15, hint16, hint17);
                            }
                            addCandidateNumbers(mass3 / 9, mass3 % 9, num3);
                        }
                    }
                    addCandidateNumbers(mass2 / 9, mass2 % 9, num2);
                }
            }
            addCandidateNumbers(mass1 / 9, mass1 % 9, num1);
        }
    }
    addHints.push_back(hint15);
    addHints.push_back(hint16);
    addHints.push_back(hint17);
    ConvergeCount17 = least_conv;
    
    /*
    bool skip[729];
    memcpy(skip, Skip, 729);
    createCandidateHints();
    
    if (deep == 3) {
        doBacktrackAll(true, ZERO, ConvergeCount17);
        if (ConvergeCount17 > Count) {
            printf("%d %d %d convCount17 %d\n", hint15, hint16, hint17, ConvergeCount17);
            Hint15To17 = std::make_tuple(hint15, hint16, hint17);
            ConvergeCount17 = Count;
        }
    } else {
        std::vector<int> candidateHints1;
        for (auto cand : CandidateHints) {
             if (cand.first < bfr) continue;
            candidateHints1.push_back(cand.first);
        }
        for (int cand : candidateHints1) {
            if (deep == 0) printf("%d\n", cand);
            discardCandidateNumbers(cand / 81, (cand / 9) % 9, cand % 9);
            switch (deep) {
                case 0:
                    hint15 = cand;
                    break;
                case 1:
                    hint16 = cand;
                    break;
                case 2:
                    hint17 = cand;
                    break;
            }
            doBacktrack15_17(deep+1, cand);
            addCandidateNumbers(cand / 81, (cand / 9) % 9, cand % 9);
        }
    }
    memcpy(Skip, skip, 729);
    */
}
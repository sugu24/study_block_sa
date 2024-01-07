#include "sudoku.hpp"


std::vector<Sudoku> sudokus;

int BEAM;
int MAX_PARENT;
                //  1~2  2~3  3~4  4~5  5~6  6~7  7~8  8~9  9~10 10~11 11~12 12~13  13~14
int SaCount[13] = { 300, 300, 300, 300, 300, 300, 300, 300, 500, 700, 700, 700, 1000 };
int BTs = 50;
int startBT = 13;
// int UnderConverve14 = 160000;
int UnderConverve14 = 1000000;
int max_14_size = 8;

bool add_three_hints;

time_t START, END;
std::string times = "";
std::string file_name;


std::vector<std::tuple<int, int, int>> setHints19 = {
    std::make_tuple( 7, 1, 0 ), std::make_tuple( 0, 5, 7 ), std::make_tuple( 8, 4, 4 ), std::make_tuple( 2, 2, 4 ), 
    std::make_tuple( 7, 0, 6 ), std::make_tuple( 6, 1, 8 ), std::make_tuple( 5, 2, 5 ), std::make_tuple( 1, 2, 2 ), 
    std::make_tuple( 0, 3, 6 ), std::make_tuple( 0, 6, 8 ), std::make_tuple( 5, 4, 2 ), std::make_tuple( 3, 6, 6 ), 
    std::make_tuple( 7, 6, 7 ), std::make_tuple( 1, 0, 5 ), std::make_tuple( 4, 0, 7 ), std::make_tuple( 4, 1, 1 )
}; // 19

std::vector<std::tuple<int, int, int>> setHints17 = {
    std::make_tuple( 8, 5, 0 ), std::make_tuple( 7, 8, 3 ), std::make_tuple( 4, 6, 8 ), std::make_tuple( 5, 1, 0 ),
    std::make_tuple( 3, 4, 6 ), std::make_tuple( 6, 0, 1 ), std::make_tuple( 1, 0, 6 ), std::make_tuple( 4, 1, 4 ),
    std::make_tuple( 4, 2, 7 ), std::make_tuple( 7, 0, 2 ), std::make_tuple( 8, 3, 4 ), std::make_tuple( 2, 6, 0 ), 
    std::make_tuple( 7, 7, 6 ), std::make_tuple( 0, 1, 5 ), std::make_tuple( 0, 7, 1 ), std::make_tuple( 1, 4, 2 ),
    std::make_tuple( 2, 3, 7 )
};

std::vector<std::tuple<int, int, int>> setHints18 = {
    std::make_tuple( 4, 1, 0 ), std::make_tuple( 1, 0, 1 ), std::make_tuple( 2, 7, 2 ), std::make_tuple( 5, 1, 2 ), 
    std::make_tuple( 0, 2, 8 ), std::make_tuple( 0, 0, 4 ), std::make_tuple( 7, 1, 6 ), std::make_tuple( 3, 8, 4 ), 
    std::make_tuple( 3, 4, 1 ), std::make_tuple( 6, 8, 1 ), std::make_tuple( 7, 5, 2 ), std::make_tuple( 0, 4, 5 ), 
    std::make_tuple( 1, 6, 6 ), std::make_tuple( 1, 7, 3 ), std::make_tuple( 5, 5, 3 ), std::make_tuple( 6, 4, 8 ), 
    std::make_tuple( 6, 7, 5 ), std::make_tuple( 1, 1, 5 )
}; // 18

bool isSetHints = false;
int setHintsNum = 0;
int setNo;

void printSudoku() {
    std::fstream f;
    f.open(file_name, std::ios::app);
    for (auto sudoku : sudokus) {
        f << sudoku.getHints().size() << " " << sudoku.getBfrId()[sudoku.getBfrId().size()-1];
        if (sudoku.getHints().size() == 14)
            f << " " << sudoku.getConvergeCount14();
        else
            f << " " << sudoku.getConv();

        f << sudoku.getBfrId();
        // for (auto hint : sudoku.getHints())
        //    f << " ( " << hint.first / 9 << " " << hint.first % 9 << " " << hint.second << " )";
        f << std::endl;
    }
    f << std::endl;
    f.close();
}

void setAppears(int HintCount, std::vector<std::tuple<int, int, int>> *appears, int beam) {
    for (int i = 0; i < sudokus.size(); i++) {
        for (auto cand : sudokus[i].getCandidateHints()) {
            if (HintCount < startBT && cand.second == 0) continue;
            appears->push_back(std::make_tuple(cand.second, i, cand.first));
        }
    }
    sort(appears->begin(), appears->end());
    
    if (HintCount < startBT) {
        int parent[6] = {0, 0, 0, 0, 0, 0};
        auto iter = appears->begin();
        while (iter != appears->end()) {
            int p = std::get<1>(*iter);
            if (parent[p] >= MAX_PARENT)
                iter = appears->erase(iter);
            else {
                parent[p]++;
                iter++;
            }
        }
    }

    appears->resize(beam);

    std::fstream f;
    f.open(file_name, std::ios::app);
    f << HintCount << " " << beam << " ->";
    for (auto appear : *appears) 
        f << " [ " << std::get<0>(appear) << " " << std::get<1>(appear) << " " << std::get<2>(appear) << " ]";
    f << std::endl;
    f.close();
}

void setConvIdHint(int *conv, int *id, int *hint, std::tuple<int, int, int> tup) {
    *conv = std::get<0>(tup);
    *id = std::get<1>(tup);
    *hint = std::get<2>(tup);
}

void setConvIdHint15Hint16Hint17(int *conv, int *id, int *hint15, int *hint16, int *hint17, std::tuple<int, int, int, int, int> tup) {
    *conv = std::get<0>(tup);
    *id = std::get<1>(tup);
    *hint15 = std::get<2>(tup);
    *hint16 = std::get<3>(tup);
    *hint17 = std::get<4>(tup);
}

void saAndAppears(int HintCount, int saTimes, std::vector<std::tuple<int, int, int>> *appears, int beam) {
    // hintの候補を調べてsaを行う
    std::fstream f;
    f.open(file_name, std::ios::app);
    for (int id = 0; id < sudokus.size(); id++) 
        f << sudokus[id].doingSATimes(saTimes) << " ";
    f << std::endl;
    f.close();
    
    // 各々のCandidateHintsをappearsへ
    // resize(beam)
    setAppears(HintCount, appears, beam);
}

// hintSizeは1つ大きい
void setExtendedtSudoku(int next_hint_size, std::vector<std::tuple<int, int, int>> *appears) {
    int conv, id, hint;
    int remove = sudokus.size();

    // appearsから新たにsudokuを生成
    for (auto appear : *appears) {
        setConvIdHint(&conv, &id, &hint, appear);
        sudokus.push_back(Sudoku(sudokus[id], id, conv, hint));
        if (next_hint_size == 14)
            sudokus[sudokus.size()-1].setConvergeCount14(conv);
    }
    for (int i = 0; i < remove; i++) 
        sudokus.erase(sudokus.begin());
}

void setSudoku(int beam) {
    sudokus.clear();

    if (isSetHints) {
        sudokus.push_back(Sudoku());
        for (int i = 0; i < setHintsNum; i++)
            if (setNo == 17)
                sudokus[0].addHint(std::get<0>(setHints17[i]), std::get<1>(setHints17[i]), std::get<2>(setHints17[i]));
            else if (setNo == 18)
                sudokus[0].addHint(std::get<0>(setHints18[i]), std::get<1>(setHints18[i]), std::get<2>(setHints18[i]));
            else if (setNo == 19)
                sudokus[0].addHint(std::get<0>(setHints19[i]), std::get<1>(setHints19[i]), std::get<2>(setHints19[i]));
            else {
                printf("error : setNo\n"); exit(1);
            }
                
        printSudoku();
    } else {
        for (int i = 0; i < beam; i++) {
            sudokus.push_back(Sudoku());
            sudokus[i].firstHint();
        }
    }
}

void hint14thsResize(int beam, std::vector<std::tuple<int, int, int>> *hint14ths) {
    int under_conv_count = 0;
    for (; under_conv_count < max_14_size; under_conv_count++)
        if (std::get<0>(hint14ths->at(under_conv_count)) >= UnderConverve14) break;
    hint14ths->resize(beam);
}

void createSudoku13(int beam) {
    // num, id, mass 
    std::vector<std::tuple<int, int, int>> appears;
    
    for (int HintCount = sudokus[0].getHints().size(); HintCount < 13; HintCount++) {
        // printf("hint count %d find %dth hint\n", HintCount, HintCount+1);
        appears.clear();

        // beam個のsudokuからsaで出現回数が少ないものを見つける
        saAndAppears(HintCount, SaCount[HintCount-1], &appears, beam);

        // appearsから新たに1つヒントに加えてたsudokuを生成
        setExtendedtSudoku(HintCount+1, &appears);

        printSudoku();
    }
}

void createSudoku14(int beam) {
    int conv, id, hint;
    std::vector<std::tuple<int, int, int>> appears; // num, id, mass

    // BTs個のsudokuからsaで出現回数が少ないものを見つける
    saAndAppears(13, SaCount[12], &appears, BTs);
    // -------------------- backtrack all -------------------- //
    // BTで解の個数数えてBEAM個に絞る
    // conv, id, mass
    std::vector<std::tuple<int, int, int>> hint14ths;

    // std::fstream f;
    // f.open(file_name, std::ios::app);
    // f << "hint14th cand ->";
    bool time_over = false;
    for (int i = 0; i < BTs; i++) {
        setConvIdHint(&conv, &id, &hint, appears.at(i));
        //Sudoku sudoku = Sudoku(sudokus[id], hint);
        sudokus[id].addHint(hint / 81, (hint / 9) % 9, hint % 9);
        time_over |= sudokus[id].AlgorithmX(ZERO) == -1;
        if (sudokus[id].getCount() == 1000000)
            continue;
        hint14ths.push_back(std::make_tuple(sudokus[id].getCount(), id, hint));
        // f << " { " << conv << " " << sudokus[id].getCount() << " }";
        sudokus[id].deleteHint(hint / 81, (hint / 9) % 9, hint % 9);
    }
    if (time_over) {
        END = time(NULL);
        std::fstream f;
        file_name = "result_algoX_.txt";
        f.open(file_name, std::ios::app);
        f << BEAM << " " << MAX_PARENT << " " << add_three_hints << " " << END - START << " time_over" << std::endl;
        f.close();
    }

    // f << std::endl;
    sort(hint14ths.begin(), hint14ths.end());

    // f << "hint14th cand ->";
    // for (auto hint14th : hint14ths)
    //     f << " [ " << std::get<0>(hint14th) << " " << std::get<1>(hint14th) << " " << std::get<2>(hint14th) << " ]";
    // f << std::endl << std::endl;
    // f.close();

    hint14thsResize(beam, &hint14ths);
    for (auto hint14th : hint14ths)
        std::cout << " [ " << std::get<0>(hint14th) << " " << std::get<1>(hint14th) << " " << std::get<2>(hint14th) << " ]";
    std::cout << std::endl;
    // -------------------- backtrack all -------------------- //

    // hint14thsから新たに1つヒントに加えてたsudokuを生成
    setExtendedtSudoku(14, &hint14ths);

    if (sudokus.size() == 0) {
        std::fstream f;
        file_name = "new_result" + std::to_string(setNo) + ".txt";
        f.open(file_name, std::ios::app);
        f << setHintsNum << " none" << std::endl;
        f.close();
        exit(1);
    }
    printSudoku();
}

void createSudoku17(int beam) {
    int conv, id, hint14;
    int hint15, hint16, hint17;
    // conv, id, hint, hint, hint
    std::vector<std::tuple<int, int, int, int, int>> hint_15th_16th_17ths;
    time_t start, end;

    // backtrack all 
    // std::fstream f;
    // f.open("algoX_bt.txt", std::ios::app);
    for (int id = 0; id < sudokus.size(); id++) {
        start = time(NULL);
        sudokus[id].AlgorithmX(THREE);
        end = time(NULL);

        if (end - START > 60 * 90) {
            END = time(NULL);
            std::fstream f;
            file_name = "result_algoX_.txt";
            f.open(file_name, std::ios::app);
            f << BEAM << " " << MAX_PARENT << " " << add_three_hints << " " << END - START << " time_over" << std::endl;
            f.close();
        }

        // f << end - start << " ) " << sudokus[id].getConvergeCount14() << std::endl;
        sudokus[id].setIndex(id);
        times += std::to_string(end - start) + " ";
        printf("sudoku getConverge %d\n", sudokus[id].getConvergeCount17());
        if (sudokus[id].getConvergeCount17() == 1)
            break;
    }
    std::cout << 1 << std::endl;
    // hint 15 16 17 を記録
    for (int id = 0; id < sudokus.size(); id++) {
        if (sudokus[id].getConvergeCount17() >= 1000000) continue;
        auto hints = sudokus[id].getAddHints();
        // f << "15 16 17 hints" << hints.at(0) << " " << hints.at(1) << " " << hints.at(2) << std::endl;
        hint_15th_16th_17ths.push_back(std::make_tuple(sudokus[id].getConvergeCount17(), id, hints.at(0), hints.at(1), hints.at(2)));
        std::cout << id << " " << sudokus[id].getConvergeCount17() << " " << hints.at(0) << " " << hints.at(1) << " " << hints.at(2) << std::endl;
    }
    // f.close();

    // 上位beam個を抽出
    sort(hint_15th_16th_17ths.begin(), hint_15th_16th_17ths.end());
    if (hint_15th_16th_17ths.size() > beam) hint_15th_16th_17ths.resize(beam);
    std::cout << 3 << std::endl;
    // f.open(file_name, std::ios::app);
    // for (auto hints : hint_15th_16th_17ths)
    //     f << std::get<0>(hints) << " " << std::get<1>(hints) << " " << std::get<2>(hints) << " " << std::get<3>(hints) << " " << std::get<4>(hints) << std::endl;
    // f.close();

    // 3つ追加したSudokuをbeam個生成
    int remove = sudokus.size();
    for (auto hint_15th_16th_17th : hint_15th_16th_17ths) {
        setConvIdHint15Hint16Hint17(&conv, &id, &hint15, &hint16, &hint17, hint_15th_16th_17th);
        sudokus.push_back(Sudoku(sudokus[id], id, conv, hint15, hint16, hint17));
    }
    std::cout << 4 << std::endl;
    for (int i = 0; i < remove; i++) sudokus.erase(sudokus.begin());
    std::cout << 5 << std::endl;
    printSudoku();

    for (auto sudoku : sudokus) {
        std::cout << "is answer ?:" << sudoku.AlgorithmX(MINUS) << std::endl;
    }
}

Sudoku createSudoku(int beam) {
    std::vector<std::tuple<int, int, int>> appears;
    int conv, id, hint;
    for (int HintCount = sudokus[0].getHints().size(); HintCount < 30; HintCount++) {
        appears.clear();
        for (int id = 0; id < sudokus.size(); id++) {
            if (sudokus[id].AlgorithmX(MINUS) == 1)
                return sudokus[id];
            
            sudokus[id].AlgorithmX(ONE);
        }
        setAppears(HintCount, &appears, beam);

        // appearsから新たに1つヒントに加えてたsudokuを生成
        setExtendedtSudoku(HintCount+1, &appears);
        // sudokus.resize(1);
        printSudoku();
    }
}

// bool underAnswer() {
//     if (sudokus[sudokus.size()-1].getConvergeCount14() < UnderConverve14) return true;
//     else return false;
// }

// arg : thisfile, out_file, beam, max_parent
int main(int argc, char *argv[]) {
    if (argc == 4) {
        BEAM = std::stoi(argv[1]);
        MAX_PARENT = std::stoi(argv[2]);
        add_three_hints = std::stoi(argv[3]) == 1;
        file_name = "sudoku" + std::to_string(BEAM) + "_" + std::to_string(MAX_PARENT) + "_.txt";
    }
    else 
        exit(printf("argc is not 4\n"));

    std::cout << BEAM << " " << MAX_PARENT << " " << file_name << " " << add_three_hints <<std::endl;
    START = time(NULL);
    setSudoku(BEAM);
    createSudoku13(BEAM);
    createSudoku14(BEAM);
    if (add_three_hints) {
        createSudoku17(BEAM);
    }
    Sudoku sudoku = createSudoku(BEAM);
    END = time(NULL);
    printSudoku();

    // ---------- result ---------- //
    std::fstream f;
    file_name = "result_algoX_.txt";
    f.open(file_name, std::ios::app);
    f << BEAM << " " << MAX_PARENT << " " << add_three_hints << " " << sudoku.getHints().size() << " " << sudoku.getConvergeCount14() << " " << END - START << " " << times << "hint ->";
    // f << sudoku.getIndex() << " " << LOOPCOUNT << " " << sudoku.getHints().size() << " " << sudoku.getConvergeCount14() << " " << END - START << " " << times << " " << sudoku.getBfrId() << std::endl;
    for (auto hint : sudoku.getHints())
        f << " ( " << hint.first / 9 << " " << hint.first % 9 << " " << hint.second << " )";
    f << std::endl;
    f.close();

}
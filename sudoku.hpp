#include <vector>
#include <map>
#include <tuple>
#include <random>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <string.h>
#include <utility>

extern int Converge14;
extern int SaCount[13];
extern int BTs;

typedef enum BACKTRACKRECORD {
    MINUS = -1,
    ZERO,
    ONE,
    TWO,
    THREE,
} BACKTRACKRECORD;

class Sudoku {
    private:
        int Mass[81];
        bool RowUsedNumber[81];
        bool ColUsedNumber[81];
        bool BlockUsedNumber[81];
        bool CandidateNumber[729];
        bool Skip[729];

        std::vector<std::tuple<int, int>> changePair;
        
        std::map<int, int> CandidateHints;
        std::vector<int> addHints;
        std::map<std::tuple<int, int, int>, int> threeHints;
        std::map<int, int> Hints;

        int Count;

        int Conv;
        std::string bfrId;
        int ConvergeCount14;
        int ConvergeCount17 = 0;
        int Ind;
    
    private:
        void init();

        // ---------- sa ---------- //
        void saInitPat();
        std::tuple<int, int> massChoice();
        int refine();
        void keepRecord();

        // ---------- backtrack ---------- //
        void recordAppearance();
        int createLeastMass();
        void backtrackAll(BACKTRACKRECORD record, int conv_limit);
        void copyCand(std::vector<int> *v, int *skip[729]);
        
        // ---------- opHints ---------- //
        void addCandidateNumbers(int row, int col, int num);
        void discardCandidateNumbers(int row, int col, int num);

        // ---------- Algorithm X ---------- //
        void recordHintSol();
        int callAlgorithmX(int state);
        void addTempNHints(int add_hint_num, struct HintNode *hint_node);
        void addTempHints(int add_hint_num, struct HintNode *hint_node, struct Node *node, struct Node *target_node);
        void eraseAndAddConditionNode2(int add_hint_num, struct HintNode *hint_node, struct ConditionNode *condition_node, struct Node *node);
        void eraseAndAddNode2(int add_hint_num, struct HintNode *hint_node1, struct HintNode *hint_node2, struct Node *node, struct Node *target_node);
        void selectHint(int add_hint_num, struct HintNode *hint_node);
        void selectAllConditionHint(struct ConditionNode *condition_node);
        void algorithmX(void);
        void preAlgorithmX(struct Node *node, struct Node *target_node);
        void eraseAndAddConditionNode1(struct ConditionNode *condition_node, struct Node *node);
        void eraseAndAddNode1(struct Node *node, struct Node *target_node);
        void setHints();
        void algorithnX_init();
        void eraseCandidateHint(struct HintNode *hint_node);
        bool isCandidateCondition(struct ConditionNode *node);
        bool isCandidateHint(struct HintNode *node);
        void addCandidateHint(struct HintNode *hint_node);
        void eraseConditionNode(struct ConditionNode *node);
        void addConditionNode(struct ConditionNode *node);
        void eraseNode(struct Node *node);
        void addNode(struct Node *node);
        void eraseHintNode(struct HintNode *node);
        void addHintNode(struct HintNode *node);
        void useHint(struct HintNode *hint_node);
        struct ConditionNode* getMinCondition();
        int isAnswer(void);
        bool freeDancingLink(void);
        struct HintNode* getHintNode(int hint);
    public:
        Sudoku() { init(); }
        Sudoku(Sudoku sudoku, int id, int conv, int hint) {
            init();
            bfrId = sudoku.getBfrId() + " " + std::to_string(id);
            Conv = conv;
            ConvergeCount14 = sudoku.getConvergeCount14();
            ConvergeCount17 = sudoku.getConvergeCount17();
            Ind = sudoku.getIndex();
            for (auto hint : sudoku.getHints())
                addHint(hint.first / 9, hint.first % 9, hint.second);
            addHint(hint / 81, (hint / 9) % 9, hint % 9);
        }
        Sudoku(Sudoku sudoku, int id, int conv, int hint15, int hint16, int hint17) {
            init();
            bfrId = sudoku.getBfrId() + " " + std::to_string(id);
            Conv = conv;
            ConvergeCount14 = sudoku.getConvergeCount14();
            ConvergeCount17 = sudoku.getConvergeCount17();
            Ind = sudoku.getIndex();
            for (auto hint : sudoku.getHints())
                addHint(hint.first / 9, hint.first % 9, hint.second);
            addHint(hint15 / 81, (hint15 / 9) % 9, hint15 % 9);
            addHint(hint16 / 81, (hint16 / 9) % 9, hint16 % 9);
            addHint(hint17 / 81, (hint17 / 9) % 9, hint17 % 9);
        }

        int getCount() { return Count; }
        std::map<int, int> getHints() { return Hints; }
        std::map<int, int> getCandidateHints() { return CandidateHints; }
        std::vector<int> getAddHints() { return addHints; }
        void setConvergeCount14(int convergeCount14) { ConvergeCount14 = convergeCount14; }
        void setIndex(int ind) { Ind = ind; }
        std::string getBfrId() { return bfrId; }
        int getConvergeCount14() { return ConvergeCount14; }
        int getConvergeCount17() { return ConvergeCount17; }
        int getIndex() { return Ind; }
        int getConv() { return Conv; }
        // int getBfrId() { return bfrId; }
        
        // ---------- sa ---------- //
        int doingSATimes(int times);

        // ---------- backtrack ---------- //
        bool backtrack();
        int doBacktrackAll(bool init_cand, BACKTRACKRECORD record, int conv_limit);
        void doBacktrack15_17();

        // ---------- opHints ---------- //
        void firstHint();
        void createCandidateHints();
        void addHint(int row, int col, int num);
        void deleteHint(int row, int col, int num);

        // ---------- Algorithm X ---------- //
        int AlgorithmX(int add_temp_hint);
};
#include "sudoku.hpp"
#include <stdio.h>
#include <map>
#include <tuple>
#include <vector>
#include <iostream>

/* 
    algorithm X で数独を解く
*/

// 条件を示す構造体
struct ConditionNode {
    struct ConditionNode *next;
    struct ConditionNode *prev;
    struct Node *head_node; // node->nextでたどっていく
    int condition;
    int number_of_node;
};

// ヒントを示す構造体
struct HintNode {
    struct HintNode *next;
    struct HintNode *prev;
    struct Node *head_node; // node->nextでたどっていく head_nodeはrightしかない
    int hint;
};

// Dancing Linkを表現する構造体
struct Node {
    struct ConditionNode *condition_node;
    struct HintNode *hint_node;
    struct Node *right;
    struct Node *left;
    struct Node *up;
    struct Node  *bottom;
};

struct ConditionNode *cond_node_head; // nextのみ
struct HintNode *hint_node_head;      // nextのみ
std::map<int, struct HintNode *> hint_node_map;
std::map<int, struct ConditionNode *> condition_node_map;
const int MAX_CONDITION_NODE = 324;
const int MAX_HINT_NODE = 729;
const int MAX_NODES_IN_CONDITION = 9;
int number_of_solutions = 0;
int min_sol = 100;
int R;
int STATE;
int algoX_mass[9][9];
int add_hints[4] = { -1, -1, -1, -1 };
long long int c = 0;

// std::vector<std::vector<std::tuple<unsigned long long int, int>>> stacks(324);

void printMass();
void printHintNode();
void printCondNode();

// ConditionNodeの削除
void Sudoku::eraseConditionNode(struct ConditionNode *node) {
    // printf("cond node %x\n", node);
    struct ConditionNode *prev_node = node->prev;
    struct ConditionNode *next_node = node->next;

    prev_node->next = next_node;
    if (next_node) next_node->prev = prev_node;
}

// ConditionNodeの追加
void Sudoku::addConditionNode(struct ConditionNode *node) {
    struct ConditionNode *prev_node = node->prev;
    struct ConditionNode *next_node = node->next;

    prev_node->next = node;
    if (next_node) next_node->prev = node;
}

// Nodeの削除 (upとbottomに対してのみ)
void Sudoku::eraseNode(struct Node *node) {
    struct Node *up_node = node->up;
    struct Node *bottom_node = node->bottom;

    up_node->bottom = bottom_node;
    if (bottom_node) bottom_node->up = up_node;

    // 属するConditionNodeのnumber_of_nodeを1つ減らす
    node->condition_node->number_of_node--;
}

// Nodeの追加（upとbottomに対してのみ）
void Sudoku::addNode(struct Node *node) {
    struct Node *up_node = node->up;
    struct Node *bottom_node = node->bottom;

    up_node->bottom = node;
    if (bottom_node) bottom_node->up = node;

    // 属するConditionNodeのnumber_of_nodeを1つ増やす
    node->condition_node->number_of_node++;

    /*
    if (std::get<0>(stacks[node->condition_node->condition].back()) == (unsigned long long int)node)
        stacks[node->condition_node->condition].pop_back();
    else {
        printf("\naddNode err node->condition_node->condition %d, stack %d\n", node->condition_node->condition, ((struct Node *)std::get<0>(stacks[node->condition_node->condition].back()))->condition_node->condition);
        printf("node->hint_node->hint %d, stack %d\n", node->hint_node->hint, ((struct Node *)std::get<0>(stacks[node->condition_node->condition].back()))->hint_node->hint);
        for (int i = 0; i < stacks[node->condition_node->condition].size(); i++) {
            printf("hint %d %d\n", ((struct Node *)std::get<0>(stacks[node->condition_node->condition][i]))->hint_node->hint, std::get<1>(stacks[node->condition_node->condition][i]));
        }
        exit(1);
    }

    if (node->condition_node->number_of_node == 1 && node->condition_node->head_node->bottom == NULL) { 
        printf("addNode condition %d, hint %d\n", node->condition_node->condition, node->hint_node->hint); 
        printf("node->up %x, node->condition_node->head_node %x\n", node->up, node->condition_node->head_node);
        exit(1); 
    }
    */
}

// HintNodeの行を削除
void Sudoku::eraseHintNode(struct HintNode *node) {
    struct HintNode *prev_node = node->prev;
    struct HintNode *next_node = node->next;

    prev_node->next = next_node;
    if (next_node) next_node->prev = prev_node;
}

// HintNodeの行を追加
void Sudoku::addHintNode(struct HintNode *node) {
    struct HintNode *prev_node = node->prev;
    struct HintNode *next_node = node->next;

    prev_node->next = node;
    if (next_node) next_node->prev = node;
}

// Hintの候補を削除
void Sudoku::eraseCandidateHint(struct HintNode *hint_node) {
    struct Node *node = hint_node->head_node;

    // HintNodeの削除
    eraseHintNode(hint_node);
    
    // HintNodeに属するNodeを削除
    while (node = node->right) {
        eraseNode(node);
    }
    return;
}

// Hintの候補を追加
void Sudoku::addCandidateHint(struct HintNode *hint_node) {
    struct Node *node = hint_node->head_node;

    // HintNodeの追加
    addHintNode(hint_node);

    // HintNodeに属するNodeを追加
    while (node = node->right)
        addNode(node);
    return;
}

inline bool Sudoku::isCandidateHint(struct HintNode *node) {
    return node->prev->next == node;
}

inline bool Sudoku::isCandidateCondition(struct ConditionNode *node) {
    return node->prev->next == node;
}


// ConditionNodeを次に移して削除して、ConditionNodeのnodeを削除する
void Sudoku::eraseAndAddConditionNode1(struct ConditionNode *condition_node, struct Node *node) {
    // ConditionNodeを削除
    eraseConditionNode(condition_node);

    // target_nodeを変更し再帰
    preAlgorithmX(node, condition_node->head_node->bottom);

    // ConditionNodeを追加
    addConditionNode(condition_node);

    return;
}

void Sudoku::eraseAndAddNode1(struct Node *node, struct Node *target_node) {
    struct HintNode *hint_node = target_node->hint_node;
    
    // HintNodeの行を削除
    eraseCandidateHint(hint_node);
    
    // target_nodeを変更し再帰
    preAlgorithmX(node, target_node->bottom);

    // HintNodeの行を追加
    // printf("(1,%d)", c);
    addCandidateHint(hint_node);

    return;
}

void debugHint();
// HintNodeに関連するHintNodeとConditionNodeを削除
void Sudoku::preAlgorithmX(struct Node *node, struct Node *target_node) {
    c++;
    if (!target_node) {
        node = node->right;
        if (!node) {
            // 全ConditionNodeを削除した
            algorithmX();
        } /* else if (!isCandidateCondition(node->condition_node)) {
            preAlgorithmX(node, NULL);
        } */ else {
            // ConditionNodeを次に移して削除して、ConditionNodeのnodeを削除する
            eraseAndAddConditionNode1(node->condition_node, node);
        }
    } else {
        eraseAndAddNode1(node, target_node);
        if (!isCandidateHint(target_node->hint_node)) { printf("err 3\n"); exit(1); }
    }
    return;
}

void Sudoku::eraseAndAddConditionNode2(int add_hint_num, struct HintNode *hint_node, struct ConditionNode *condition_node, struct Node *node) {
    // ConditionNodeを削除
    // printf("2 cond node %x\n", condition_node);
    eraseConditionNode(condition_node);

    // target_nodeを変更し再帰
    addTempHints(add_hint_num, hint_node, node, condition_node->head_node->bottom);

    // ConditionNodeを追加
    addConditionNode(condition_node);

    return;
}

void Sudoku::eraseAndAddNode2(int add_hint_num, struct HintNode *hint_node1, struct HintNode *hint_node2, struct Node *node, struct Node *target_node) {
    // HintNodeの行を削除
    eraseCandidateHint(hint_node2);
    
    // target_nodeを変更し再帰
    addTempHints(add_hint_num, hint_node1, node, target_node->bottom);

    // HintNodeの行を追加
    addCandidateHint(hint_node2);

    return;
}


void Sudoku::addTempHints(int add_hint_num, struct HintNode *hint_node, struct Node *node, struct Node *target_node) {
    if (!target_node) {
        node = node->right;
        if (!node) {
            // 全ConditionNodeを削除した
            addTempNHints(add_hint_num-1, hint_node);
        } else {
            // ConditionNodeを次に移して削除して、ConditionNodeのnodeを削除する
            eraseAndAddConditionNode2(add_hint_num, hint_node, node->condition_node, node);
        }  
    } else {
        // printf("hint_node->hint %d\n", target_node->hint_node->hint);
        if (!isCandidateHint(target_node->hint_node)) { printf("err 1 %d target_node->hint_node %d %d\n", add_hint_num, target_node->hint_node->hint, target_node->condition_node->condition); exit(1); }
        eraseAndAddNode2(add_hint_num, hint_node, target_node->hint_node, node, target_node);
    }
    return;
}

void Sudoku::selectHint(int add_hint_num, struct HintNode *hint_node) {
    struct Node *head_node = hint_node->head_node;
    int row = hint_node->hint / 81;
    int col = (hint_node->hint / 9) % 9;
    int num = hint_node->hint % 9;

    // Mass[row][col] = num;
    
    // HintNodeに関連するHintNodeとConditionNodeを削除
    if (add_hint_num)
        addTempHints(add_hint_num, hint_node, head_node, NULL);
    else
        preAlgorithmX(head_node, NULL);

    // Mass[row][col] = 9;

    return;
}

// ConditionNodeを満たすHint全てを順に使用する
void Sudoku::selectAllConditionHint(struct ConditionNode *condition_node) {
    struct Node *node = condition_node->head_node;
    struct HintNode *nodes[MAX_NODES_IN_CONDITION];
    int nodes_in_condition;
    
    while (node = node->bottom) {
        if (R <= number_of_solutions) return;
        selectHint(0, node->hint_node);
    }
    
    return;
}

void Sudoku::algorithmX(void) {
    if (R <= number_of_solutions) return;

    if (isAnswer()) {
        number_of_solutions++;
    } else {
        struct ConditionNode *condition_node;
        if (condition_node = getMinCondition()) {
            if (condition_node->number_of_node > 0) { 
                selectAllConditionHint(condition_node); // selectAllNodeを進めていき、その中でAlgorithmXを呼び出す
            }
        }
    }
    return;
}

void Sudoku::recordHintSol() {
    if (STATE == THREE) {
        Hint15To17 = std::make_tuple(add_hints[0], add_hints[0], add_hints[0]);
        ConvergeCount17 = number_of_solutions;
    } else if (STATE == ONE) {
        CandidateHints[add_hints[0]] = number_of_solutions;
    }
}

int Sudoku::callAlgorithmX(int state) {
    number_of_solutions = 0;
    if (STATE == ZERO)
        R = state * 1700000 + !state;
    else
        R = state * min_sol + !state;
    
    algorithmX();
    if (STATE == ZERO)
        Count = number_of_solutions;
    else if (state && number_of_solutions && min_sol > number_of_solutions) {
        printf("sol : %d -> %d\n", min_sol, number_of_solutions);
        min_sol = number_of_solutions;
        if (STATE == THREE) {
            printf("solution : %d -> %d\n", min_sol, number_of_solutions);
            Hint15To17 = std::make_tuple(add_hints[0], add_hints[1], add_hints[2]);
            ConvergeCount17 = number_of_solutions;
        }
    }
    
    return number_of_solutions;
}

// 仮のヒントを添加してadd_hint_num個添加したらalgorithmXを行い解の個数を調べる
// arg: add_hint_num: 添加する個数, hint_node: 先ほど添加したHintNode
void Sudoku::addTempNHints(int add_hint_num, struct HintNode *hint_node) {
    if (add_hint_num) {
        // 解があるなら続行
        if (!callAlgorithmX(0)) return;
        
        while (hint_node = hint_node->next) {
            if (!isCandidateHint(hint_node)) continue;
            if (add_hint_num == 3) printf("add_hint_num 3 %d\n", hint_node->hint);
            add_hints[add_hint_num-1] = hint_node->hint;
            selectHint(add_hint_num, hint_node);
            add_hints[add_hint_num-1] = -1;
        }
    } else {
        // printf("===============\n");
        callAlgorithmX(1);
        // printf("!!!!!!!!!!!!!!!\n");
    }
    return;
}

int setNum = 14;

/*typedef enum BACKTRACKRECORD {
    ZERO,
    ONE,
    TWO,
    THREE
} BACKTRACKRECORD;
*/
void Sudoku::AlgorithmX(int add_temp_hint) {
    STATE = add_temp_hint;
    if (add_temp_hint == ONE)
        min_sol = 1;
    else min_sol = 1000;

    // Dancing Linkの初期化
    algorithnX_init();

    // Hintをセット
    setHints();

    addTempNHints(add_temp_hint, hint_node_head->next);

    if (!freeDancingLink()) {
        printf("freeDancingLink() falure\n");
        exit(1);
    } else
        printf("freeDancingLink() success\n");
}
/*
int main() {
    // Dancing Linkの初期化
    init();

    time_t start = time(NULL);
    
    // Hintをセット
    setHints(setHints14);
    
    addTempNHints(17-setNum, hint_node_head->next);

    if (!freeDancingLink()) {
        printf("freeDancingLink() falure\n");
        return -1;
    } else
        printf("freeDancingLink() success\n");

    time_t end = time(NULL);

    printf("solution %d\n", number_of_solutions);

    printf("time %ld\n", end - start);
    
    printf("c %lld\n", c);
    return 0;
}
*/
void Sudoku::algorithnX_init() {
    struct ConditionNode *cond_node, *next_cond_node;
    struct HintNode *hint_node, *next_hint_node;
    struct Node *node, *next_node;
    std::map<int, struct Node *> condition_head_node_map;

    std::cout << "---------- init start ----------" << std::endl;

    for (int i = 0; i < 9; i++) for (int j = 0;j < 9; j++) algoX_mass[i][j] = 9;

    // headを生成
    cond_node_head = (struct ConditionNode *)calloc(1, sizeof(struct ConditionNode));
    hint_node_head = (struct HintNode *)calloc(1, sizeof(struct HintNode));

    cond_node = cond_node_head;
    hint_node = hint_node_head;

    // HintNodeの生成
    for (int i = 0; i < MAX_HINT_NODE; i++) {
        next_hint_node = (struct HintNode *)calloc(1, sizeof(struct HintNode));

        next_hint_node->hint = i;
        hint_node->next = next_hint_node;
        next_hint_node->prev = hint_node;
        next_hint_node->head_node = (struct Node *)calloc(1, sizeof(struct Node));

        hint_node = next_hint_node;

        hint_node_map[i] = hint_node;
    }
    
    // ConditionNodeの生成（マスの使用(1~81)、行(82~162)、列(163~243)、ブロック(244~324)）
    for (int i = 0; i < MAX_CONDITION_NODE; i++) {
        next_cond_node = (struct ConditionNode *)calloc(1, sizeof(struct ConditionNode));
        next_cond_node->condition = i;
        next_cond_node->number_of_node = 0;

        cond_node->next = next_cond_node;
        next_cond_node->prev = cond_node;

        cond_node = next_cond_node;

        cond_node->head_node = (struct Node *)calloc(1, sizeof(struct Node));

        condition_node_map[i] = cond_node;
        condition_head_node_map[i] = cond_node->head_node;
    }
    
    // Nodeの生成 hint_nodeのNodeを生成していく
    hint_node = hint_node_head;
    while (hint_node = hint_node->next) {
        node = hint_node->head_node;
        // printf("hint : %d\n", hint_node->hint);
        int hint = hint_node->hint; // row * col * num
        int mass = hint / 9;
        int row_num = (mass / 9) * 9 + (hint % 9);
        int col_num = (mass % 9) * 9 + (hint % 9);
        int block_num = ((mass / 27) * 3 + (mass % 9) / 3) * 9 + (hint % 9);
        for (int i = 0; i < MAX_CONDITION_NODE; i++) {
            if (mass == i || row_num == (i - 81) || col_num == (i - 162) || block_num == (i - 243)) {
                next_node = (struct Node *)calloc(1, sizeof(struct Node));

                // right leftの設定
                node->right = next_node;
                next_node->left = node;
                node = next_node;
                
                // nodeが属するhint_nodeとcondition_nodeを設定
                node->hint_node = hint_node;
                node->condition_node = condition_node_map[i];
                node->condition_node->number_of_node++;
                
                // up bottomの設定
                condition_head_node_map[i]->bottom = node;
                node->up = condition_head_node_map[i];
                condition_head_node_map[i] = node;
            }
        }
    }

    std::cout << "---------- init end ----------" << std::endl;
}

bool Sudoku::freeDancingLink(void) {
    struct Node *prev_node, *now_node;

    for (int i = 0; i < MAX_HINT_NODE; i++) {
        now_node = hint_node_map[i]->head_node;
        while (prev_node = now_node) {
            now_node = now_node->right;
            free(prev_node);
            prev_node = NULL;
        }
        free(hint_node_map[i]);
        hint_node_map[i] = NULL;
    }
    for (int i = 0; i < MAX_CONDITION_NODE; i++) {
        free(condition_node_map[i]->head_node);
        free(condition_node_map[i]);
        condition_node_map[i]->head_node = NULL;
        condition_node_map[i] = NULL;
    }
    free(cond_node_head);
    free(hint_node_head);
    cond_node_head = NULL;
    hint_node_head = NULL;
    return true;
}

struct HintNode* Sudoku::getHintNode(int hint) {
    struct HintNode *hint_node = hint_node_head;

    while (hint_node = hint_node->next) {
        if (hint_node->hint == hint) {
            return hint_node;
        }
    }
    printf("none hint\n");
    return NULL;
}

void Sudoku::useHint(struct HintNode *hint_node) {
    struct Node *node_in_hint = hint_node->head_node;
    struct Node *node = NULL;

    algoX_mass[hint_node->hint / 81][(hint_node->hint / 9) % 9] = hint_node->hint % 9;

    while (node_in_hint = node_in_hint->right) {
        node = node_in_hint->condition_node->head_node;
        eraseConditionNode(node_in_hint->condition_node);
        while (node = node->bottom) {
            eraseCandidateHint(node->hint_node);
        }
    }

    return;
}

enum { CONTINUE = 0, SOLUTION = 1 };
// Dancing Linkの状態を返す
int Sudoku::isAnswer(void) {
    if (cond_node_head->next == NULL && hint_node_head->next == NULL)
        return SOLUTION;
    else
        return CONTINUE;
}

// 最小のnumber_of_nodeを持つConditionNodeを返す
struct ConditionNode* Sudoku::getMinCondition() {
    struct ConditionNode *min_condition_node = NULL, *condition_node;
    int min_number_of_node = MAX_HINT_NODE;

    condition_node = cond_node_head;
    while (condition_node = condition_node->next) {
        if (condition_node->number_of_node == 0)
            return condition_node;
        else if (min_number_of_node > condition_node->number_of_node) {
            min_number_of_node = condition_node->number_of_node;
            min_condition_node = condition_node;
        }
    }
    return min_condition_node;
}

void Sudoku::setHints() {
    struct HintNode *hint_node;
    int hint;
    
    std::cout << "---------- set hint start ----------" << std::endl;
    int i = 0;
    printf("%ld\n", getHints().size());
    for (auto hint_ele : getHints()) {
        hint = hint_ele.first * 9 + hint_ele.second;
        printf("hint %d\n", hint);
        if (hint_node = getHintNode(hint)) { /* printMass(); printHintNode(); printCondNode(); */
            useHint(hint_node);
        } else
            exit(printf("none hint node %d\n", hint));
    }
    std::cout << "---------- set hint end ----------" << std::endl;
}

// void printMass() {
//     for (int i = 0; i < 9; i++) {
//         for (int j = 0; j < 9; j++) 
//             printf("%d ", algoX_mass[i][j]);
//         printf("\n");
//     }
//     printf("\n");
// }

// void printHintNode() {
//     struct HintNode *hint_node = hint_node_head;
//     int count = 0;

//     printf("hint_node ");
//     while (hint_node = hint_node->next) {
//         printf("%d ", hint_node->hint);
//         count++;
//     }
//     printf("\n hint_node count : %d\n\n", count);
// }

// void printCondNode() {
//     struct ConditionNode *cond_node = cond_node_head;
//     int count = 0;

//     printf("cond_node ");
//     while (cond_node = cond_node->next) {
//         printf("%d ", cond_node->condition);
//         count++;
//     }
//     printf("\n cond_node count : %d\n\n", count);
// }

// void MassIsSolution() {
//     printf("%d\n", number_of_solutions);
//     int check[3] = {0,0,0};
//     for (int i = 0; i < 9; i++) {
//         int row_check[9] = { 0,0,0,0,0,0,0,0,0 };
//         int col_check[9] = { 0,0,0,0,0,0,0,0,0 };
//         int block_check[9] = { 0,0,0,0,0,0,0,0,0 };
//         for (int j = 0; j < 9; j++) {
//             row_check[algoX_mass[i][j]]++;
//             col_check[algoX_mass[j][i]]++;
//             block_check[algoX_mass[i/3*3+j/3][i%3*3+j%3]]++;
//             printf("%d ", algoX_mass[i][j]);
//         }
//         for (int j = 0; j < 9; j++) {
//             if (row_check[j] == 0) check[0]++;
//             if (col_check[j] == 0) check[1]++;
//             if (block_check[j] == 0) check[2]++;
//         }
//         printf("\n");
//     }
//     printf("\n%d %d %d\n\n", check[0], check[1], check[2]);
// }

// void debugHint() {
//     struct Node *node;

//     for (int i = 0; i < MAX_HINT_NODE; i++) {
//         node = hint_node_map[i]->head_node;
//         while (node = node->right) {
//             if (isCandidateHint(node->hint_node) != (node->up->bottom == node)) {
//                 printf("err hint_node %d, condition_node %d\n", node->hint_node->hint, node->condition_node->condition);
//                 exit(1);
//             }
//         }
//     }
// }
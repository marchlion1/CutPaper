#include "BasicJsonIO.hpp"
#include "Copoment.hpp"
#include "DfsBlock.hpp"
#include "Distrubute.hpp"
#include "CurOrder.hpp"
#include <cassert>

#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace std;
vector<Board *> boards;
const int board_num = 21;

void store(Board *test, const Method &method) {
    for (auto place: method.best) {
        if (place.is_rotated && !place.block->is_rotated) {
            Rotate(place.block);
        }
        test->consist.emplace_back(place.block, place.pos);
        place.block->used = true;
    }
    boards.push_back(test);
}

void storeFinal(distributeMethod &distribute) {
    int cnt =0 ;
    for (const auto &method: distribute.methods) {
        auto *board = new Board;
        store(board, method);
        cout <<"for "<<cnt<<":"<<endl;
        method.show();
        cnt ++;
    }
    cout << "the final store value " << distribute.getValue() << endl;
}

void setAllUsed(Method &method) {
    for (auto place: method.best) {
        place.block->used = true;
    }
}

int main() {
    time_t start_time = clock();
    srand((int) (1000));
    init();
    int kind_name = 1;
    double tot_sqrt = 0;
    for (auto block: Copoment::total_set[kind_name]) {
        tot_sqrt += block->getSqrt();
    }
    double rate = (tot_sqrt / (board_num * Config::height * Config::width));
    cout << " use rate " << rate << endl;
    vector<Copoment *> pool = Copoment::total_set[kind_name];
    distributeMethod init_method;
    for (int i = 0; i < 30; i++) {
        Method A = getDfsMethod(pool);
        if (A.best.empty())
            break;
        cout <<"find "<<endl;
        for(auto place:A.best){
            cout << place.block->id << " ";
        }
        cout << endl << "use rate " << A.getUseRate()<< endl;
        init_method.methods.push_back(A);
        vector<Copoment *> used_pool;
        for (auto place: A.best) {
            used_pool.push_back(place.block);
        }
        init_method.board_have.push_back(used_pool);
        setAllUsed(A);
        cout << ":" << i + 1 << " block num " << A.best.size() << endl;
        cout << "==========\n";
    }
    for (auto block: pool) {
        block->used = false;
    }
    init_method.setValue();
    cout << init_method.getValue() << endl;

    distributeMethod best_method = init_method;

    int dfs_num = 20000;
    struct cmpByValue {
        bool operator()(const distributeMethod &lhs, const distributeMethod &rhs) {
            return lhs.getValue() > rhs.getValue();
        }
    };
    set<distributeMethod, cmpByValue> distributes;
    distributes.insert(best_method);
    int size_lim = 5;
    int pre_dfs = 0;
    int count_pre = 0;
    while ((--dfs_num) > 0 && !distributes.empty()) {
        auto most_value_method = (*distributes.begin());
        distributes.erase(distributes.begin());
        if (dfs_num % 500 == 0) {
            cout << dfs_num << " size " << size_lim << " cur best " << best_method.getValue() << endl;
        }
        if (most_value_method.getValue() > best_method.getValue()) {
            best_method = most_value_method;
            size_lim = min((int) (size_lim * 1.2), 2000);
            pre_dfs = dfs_num;
            count_pre = dfs_num;
        }
        if (abs(pre_dfs - dfs_num) > 500) {
            most_value_method = best_method;
            pre_dfs = dfs_num;
        }
        for (const auto &method: most_value_method.neighbor()) {
            distributes.insert(method);
        }
        while (distributes.size() > size_lim) {
            distributes.erase(--distributes.end());
        }
    }
    best_method.setValue();
    for (Method &method: best_method.methods) {
        setpriority(method);
    }
    storeFinal(best_method);
    store_result(result_path, boards);
    show_rest(kind_name);
    for (auto pool: Copoment::total_set) {
        for (auto block: pool.second) {
            delete block;
        }
    }
    for (auto board: boards) {
        delete board;
    }
    time_t end_time = clock();
    cout <<"cost "<< end_time - start_time << endl;
}
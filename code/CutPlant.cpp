#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"
#include "BasicJsonIO.hpp"
#include "Copoment.hpp"
#include "DfsBlock.hpp"
#include "Distrubute.hpp"
#include "CurOrder.hpp"
#include "SKLmethod.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

void setAllUsed(Method &method) {
    for (auto place: method.best) {
        place.block->used = true;
    }
}

int main() {
    time_t start_time = clock();
    srand((int) (0));
    inputData();
    int kind_name = 1;
    double tot_sqrt = 0;
    for (auto block: Copoment::total_set[kind_name]) {
        tot_sqrt += block->getSqrt();
    }
    vector<Copoment *> pool = Copoment::total_set[kind_name];
    distributeMethod init_method;
    for (int i = 0; i < 30; i++) {
        DfsPara default_para;
        Method A = getDfsMethod(pool,default_para);
        if (A.best.empty())
            break;
        cout << "find " << endl;
        for (auto place: A.best) {
            cout << place.block->id << " ";
        }
        cout << endl << "use rate " << A.getUseRate() << endl;
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
    distributeMethod best_method = init_method;
    int board_num = best_method.board_have.size();
    cout <<" cur board num is "<< board_num << endl;
    for(int i =1;i<=3;i++){
        if(Config::getBoardSqrt() * (board_num-i) < getSumPool(pool)){
            break;
        }
        best_method = tryWithBoardNum(board_num - i, pool,best_method);
    }
    for (Method &method: best_method.methods) {
        setPriority(method);
    }
    storeDistrubute(best_method);
    checkRestBlock(kind_name);
    for (const auto& set: Copoment::total_set) {
        for (auto block: set.second) {
            delete block;
        }
    }
    time_t end_time = clock();
    cout << "cost " << (double)(end_time - start_time)*1e-3<<"(s)" << endl;
}
#pragma clang diagnostic pop
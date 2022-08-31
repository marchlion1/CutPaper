#ifndef DISTRBUTE
#define DISTRBUTE

#include <random>

#include "DfsBlock.hpp"

class distributeMethod {
public:
    vector<Method> methods;
    vector<vector<Copoment *>> board_have;
    double lowest_userates = 0;
    double getValue() const {
        return lowest_userates;
    }
    void setValue() {
        lowest_userates = 0;
        for (const auto& method : methods) {
            double use_rate = method.getUseRate();
            lowest_userates += use_rate * use_rate;
        }
    }
    vector<distributeMethod> neighbor() {
        vector<distributeMethod> distributeMethods;
        assert(methods.size() == board_have.size());
        for (int i = 0; i < methods.size(); i++) {
            if (board_have.empty())
                continue;
            int board_one = i;
            int board_two = rand() % board_have.size();
            if (board_one == board_two)
                continue;
            if (methods[board_one].getUseRate() > 0.99 && methods[board_two].getUseRate() > 0.99) {
                continue;
            }
            int siz = board_have[board_one].size();
            if (siz == 0)
                continue;
            int block_id = rand() % siz;
            auto block = board_have[board_one][block_id];
            distributeMethod new_method = (*this);
            swap(new_method.board_have[board_one][block_id], new_method.board_have[board_one][siz - 1]);
            new_method.board_have[board_two].push_back(block);
            new_method.board_have[board_one].pop_back();
            new_method.methods[board_one] = getDfsMethod(new_method.board_have[board_one]);
            new_method.methods[board_two] = getDfsMethod(new_method.board_have[board_two]);
            new_method.setValue();
            distributeMethods.push_back(new_method);
        }
        return distributeMethods;
    }
};

vector<Copoment*> pickBlockSumSqrtAs(double sqrt,vector<Copoment*>& pool){
    vector<Copoment*> best_fit_set;
    double best_dif = sqrt;
    int random_times = 20;
    while(random_times--){
        vector<Copoment*> cur_fit_set;
        shuffle(pool.begin(),pool.end(), std::mt19937(std::random_device()()));
        double cur_sqrt = 0 ;
        for(auto block: pool){
            if(block->shape.getSqrt() + cur_sqrt < sqrt){
                cur_sqrt += block->shape.getSqrt();
                cur_fit_set.push_back(block);
            }
        }
        if(sqrt - cur_sqrt < best_dif){
            best_fit_set = cur_fit_set;
            best_dif = sqrt - cur_sqrt;
        }
    }
}


#endif
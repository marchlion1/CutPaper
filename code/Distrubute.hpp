#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
#ifndef DISTRBUTE
#define DISTRBUTE

#include <random>

#include "DfsBlock.hpp"
#include "MaxFlow.hpp"

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
        for (const auto &method: methods) {
            double use_rate = method.getSorce();//method.getUseRate();
            lowest_userates += use_rate;
        }
    }

    bool isFullyUsed() const {
        assert(board_have.size() == methods.size());
        bool sud = true;
        for (int i = 0; i < board_have.size(); i++) {
            set<int> haved;
            for (auto block: board_have[i]) {
                haved.insert(block->id);
            }
            for (auto block: methods[i].best) {
                haved.erase(block.block->id);
            }
            if (!haved.empty()) {
                sud = false;
                cout <<" for "<<i<<" don't have \n";
                for(auto id:haved){
                    cout <<id <<" ";
                }
                cout <<endl;
            }
        }
        return sud;
    }

    void show() {
        int cnt = 0;
        for (auto method: methods) {
            cout << "in board " << (cnt++) << endl;
            method.show();
            double block_sqrt = 0;
            for (auto block: board_have[cnt - 1]) {
                cout << block->id << " ";
                block_sqrt += block->getSqrt();
            }
            cout << " with sqrt " << block_sqrt;
            cout << endl << "==========" << endl;
        }
        cout << " tot value " << getValue() << endl;
    }

    bool SqrtCheckPass();

    vector<distributeMethod> neighbor() {
        vector<distributeMethod> distributeMethods;
        assert(methods.size() == board_have.size());
        for (int i = 0; i < methods.size(); i++) {
            if (board_have[i].empty()) {
                for (auto place: methods[i].best) {
                    board_have[i].push_back(place.block);
                }
            }
            int board_one = i;
            int board_two = rand() % board_have.size();
            if (board_one == board_two)
                continue;
            if (methods[board_one].getUseRate() > 0.99 && methods[board_two].getUseRate() > 0.99) {
                continue;
            }
            if (methods[board_one].getUseRate() < 0.8) {
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
            DfsPara para;
            new_method.methods[board_one] = getDfsMethod(new_method.board_have[board_one],para);
            new_method.methods[board_two] = getDfsMethod(new_method.board_have[board_two],para);
            new_method.setValue();
            distributeMethods.push_back(new_method);
        }
        return distributeMethods;
    }
    distributeMethod getIterByMaxFlow(int rand_div, int board_num);

    void mutate(int step);

    void mutate2(int step);

    void mutate3();
};

vector<Copoment *> pickBlockSumSqrtAs(double sqrt, vector<Copoment *> &pool) {
    vector<Copoment *> best_fit_set;
    double best_dif = sqrt;
    int random_times = 10;
    while (random_times--) {
        /*
         * for random sqrt of block
         * shuffle + greedy will achieve a near best result cmp to DP
         */
        vector<Copoment *> cur_fit_set;
        shuffle(pool.begin(), pool.end(), std::mt19937(std::random_device()()));
        double cur_sqrt = 0;
        double cur_dif = sqrt;
        for (auto block: pool) {
            if (abs(block->shape.getSqrt() + cur_sqrt - sqrt) < cur_dif) {
                cur_sqrt += block->shape.getSqrt();
                cur_fit_set.push_back(block);
                cur_dif = abs(cur_sqrt - sqrt);
            }
        }
        if (abs(sqrt - cur_sqrt) < best_dif) {
            best_fit_set = cur_fit_set;
            best_dif = abs(sqrt - cur_sqrt);
        }
    }
    set<int> erase_list;
    for (auto block: best_fit_set) {
        erase_list.insert(block->id);
    }
    vector<Copoment *> new_pool;
    for (auto block: pool) {
        if (erase_list.find(block->id) == erase_list.end()) {
            new_pool.push_back(block);
        }
    }
    int n = pool.size();
    pool = new_pool;
    assert(n == (pool.size() + best_fit_set.size()));
    return best_fit_set;
}

distributeMethod distributeMethod::getIterByMaxFlow(int rand_div, int board_num) {
    double sqrt = Config::height * Config::width / (rand_div + 1);
    vector<vector<Copoment *>> picked_blocks;
    for (auto &pool: board_have) {
        assert(!pool.empty());
        auto picked_pool = pickBlockSumSqrtAs(sqrt, pool);
        assert(picked_pool.size() + pool.size() > 0);
        picked_blocks.push_back(picked_pool);
    }
    assert(board_have.size() == picked_blocks.size());
    int start_point = 0;
    int end_point = board_num * 2 + 1;
    ZKW_MinCostMaxFlow G;
    G.init();
    for (int pick_id = 1; pick_id <= board_num; pick_id++) {
        G.addedge(start_point, pick_id, 1, 0);
    }
    for (int board_id = board_num + 1; board_id <= board_num * 2; board_id++) {
        G.addedge(board_id, end_point, 1, 0);
    }
    DfsPara local_dfs_para;

    local_dfs_para.dfs_when_line_1 = 5;
    local_dfs_para.dfs_when_line_2 = 3;
    local_dfs_para.dfs_when_line_3 = 1;
    local_dfs_para.limit_dfs_deep = 20;

    for (int pick_id = 1; pick_id <= board_num; pick_id++) {
        for (int board_id = board_num + 1; board_id <= board_num * 2; board_id++) {
            vector<Copoment *> possible_pool(picked_blocks[pick_id - 1]);
            for (auto block: board_have[board_id - (board_num + 1)]) {
                possible_pool.push_back(block);
            }
            Method possible_method = getDfsMethod(possible_pool, local_dfs_para);
            double use_rate = possible_method.getSorce();
            int value = (int) (-use_rate * 1e6);
            G.addedge(pick_id, board_id, 1, value);
        }
    }
    vector<Match> result = G.getMatch(0, board_num * 2 + 1, board_num * 2 + 2, board_num);
    assert(result.size() == board_num);

    distributeMethod new_distrubute;
    for (int i = 0; i < board_num; i++) {
        auto [pick_id, board_id] = result[i];
        vector<Copoment *> new_pool(picked_blocks[pick_id - 1]);
        for (auto block: board_have[board_id - (board_num + 1)]) {
            new_pool.push_back(block);
        }
        assert(!new_pool.empty());
        Method method = getDfsMethod(new_pool, local_dfs_para);
        new_distrubute.methods.push_back(method);
        new_distrubute.board_have.push_back(new_pool);
    }
    new_distrubute.setValue();
    return new_distrubute;
}

void distributeMethod::mutate(int step) {
    while (step--) {
        int minBoard_id = -1, maxBoard_id = -1;
        double minSqrt = 1e18, maxSqrt = 0;
        for (int i = 0; i < board_have.size(); i++) {
            assert(!board_have[i].empty());
            double sqrt = getSumPool(board_have[i]);
            if (sqrt < minSqrt ) {
                minSqrt = sqrt;
                minBoard_id = i;
            }
            if (sqrt > maxSqrt ) {
                maxSqrt = sqrt;
                maxBoard_id = i;
            }
        }
        assert(maxBoard_id != -1 && minBoard_id != -1);
        assert(!board_have[maxBoard_id].empty() && !board_have[minBoard_id].empty());
        int rand_id = rand() % board_have[maxBoard_id].size();
        int siz = board_have[maxBoard_id].size();
        Copoment *block = board_have[maxBoard_id][rand_id];
        swap(board_have[maxBoard_id][rand_id], board_have[maxBoard_id][siz - 1]);
        board_have[maxBoard_id].pop_back();
        board_have[minBoard_id].push_back(block);
    }
}

distributeMethod getBetter(const distributeMethod &ref_distrubute) {
    distributeMethod cur_distrubute = ref_distrubute;
    int times = 320;
    double pre_value = 0;
    while (times--) {
        int rand_div = rand() & 1 ? (rand() % 20 + 20) : (rand() % 12);
        int board_num = cur_distrubute.board_have.size();
        distributeMethod new_method = cur_distrubute.getIterByMaxFlow(rand_div, board_num);
        cur_distrubute = new_method;
        if (times % 80 == 0) {
            if (pre_value + eps > cur_distrubute.getValue()) {
                cout << "break!\n";
                break;
            }
            pre_value = cur_distrubute.getValue();
            cout << cur_distrubute.getValue() << " is value " << " when " << times << endl;
        }
    }
    cur_distrubute.setValue();





    return cur_distrubute;
}


bool distributeMethod::SqrtCheckPass() {
    for (auto &i: board_have) {
        if (getSumPool(i) > Config::getBoardSqrt())return false;
    }
    return true;
}

void distributeMethod::mutate2(int step) {
    while (step--) {
        int minBoard_id = -1, maxBoard_id = -1;
        double minSqrt = 1e18, maxSqrt = 0;
        for (int i = 0; i < board_have.size(); i++) {
            assert(!board_have[i].empty());
            double sqrt = getSumPool(board_have[i]);
            if (sqrt < minSqrt) {
                minSqrt = sqrt;
                minBoard_id = i;
            }
            if (sqrt > maxSqrt) {
                maxSqrt = sqrt;
                maxBoard_id = i;
            }
        }
        assert(maxBoard_id != -1 && minBoard_id != -1);
        assert(!board_have[maxBoard_id].empty() && !board_have[minBoard_id].empty());
        int rand_id = rand() % board_have[maxBoard_id].size();
        int siz = board_have[maxBoard_id].size();
        Copoment *block = board_have[maxBoard_id][rand_id];
        swap(board_have[maxBoard_id][rand_id], board_have[maxBoard_id][siz - 1]);
        board_have[maxBoard_id].pop_back();
        board_have[minBoard_id].push_back(block);
    }
}

void distributeMethod::mutate3() {
    vector<Copoment *> redistribute;
    for (int i = 0; i < board_have.size(); i++) {
        if (methods[i].best.size() < board_have[i].size()) {
            cout << "unmathc in board " << i << endl;
            set<int> block_id;
            for (auto &place: methods[i].best) {
                block_id.insert(place.block->id);
            }
            vector<Copoment *> new_board_have;
            for (auto &block: board_have[i]) {
                if (block_id.find(block->id) == block_id.end()) {
                    redistribute.push_back(block);
                } else {
                    new_board_have.push_back(block);
                }
            }
            swap(board_have[i], new_board_have);
        }
    }
    for (auto block: redistribute) {
        int rand_id = rand() % board_have.size();
        board_have[rand_id].push_back(block);
    }

}

bool P(double p) {
    if (p <= 0)return false;
    if (p >= 1)return true;
    const int range = 1e5;
    int pint = (int) (p * range);
    return pint > (rand() * rand() % range);
}

distributeMethod tryWithBoardNum(int board_num, vector<Copoment *> &pool, distributeMethod &ref_distribute) {
    assert(board_num > 0);
    cout << " try on " << board_num << endl;
    vector<double> board_sqrt(board_num, 0);
    distributeMethod init_distribute;
    init_distribute.board_have.resize(board_num);
    int siz = ref_distribute.board_have.size();
    for (int i = 0; i < board_num; i++) {
        init_distribute.board_have[i] = ref_distribute.board_have[i];
    }
    for (int i = board_num; i < siz; i++) {
        for (auto block: ref_distribute.board_have[i]) {
            init_distribute.board_have[rand() % (board_num)].push_back(block);
        }
    }
    int iter_times = 8;

    bool sud = false;
    double T = 3, r = 0.8;

    while (iter_times-- && !sud) {
        int t_times = 4;
        cout << "in T " << T << endl;
        while (t_times--) {
            distributeMethod orgin_distrubute = init_distribute;
            init_distribute.mutate(1);
            int max_limit = 40;
            while (!init_distribute.SqrtCheckPass() && (max_limit--) > 0)init_distribute.mutate(1);
            cout << " for " << iter_times << " and " << t_times << endl;
            init_distribute = getBetter(init_distribute);
            double dif_value = init_distribute.getValue() - orgin_distrubute.getValue();
            if (init_distribute.isFullyUsed()) {
                cout << "sud find a distrubute " << endl;
                sud = true;
                break;
            }
            if (dif_value > 0) {
                cout << " find better " << endl;
                continue;
            } else if (P(exp(dif_value / T))) {
                cout <<" back to "<< orgin_distrubute.getValue()<<endl;
                init_distribute = orgin_distrubute;
            } else {
                cout << "rand stay " << endl;
            }
        }
        T = T * r;
    }
    return init_distribute;
}

#endif
#pragma clang diagnostic pop
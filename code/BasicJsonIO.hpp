#ifndef BASICJSONIO_H
#define BASICJSONIO_H
#include "Copoment.hpp"
#include "json.hpp"
#include "Distrubute.hpp"
#include <ctime>
using namespace std;
const string data_path = "../../data/data/data/data_1.json";
const string result_path = "../../data/result.json";
using json = nlohmann::json;

void storeBoardsToFile(const vector<shared_ptr<Board>> &boards, const string &file_name) {
    json final_json;
    map<string, vector<json>> board_jsons;
    map<string, vector<json>> board_infos;
    double tot_sqrt = 0, used_sqrt = 0;
    for (auto board : boards) {
        if (board->consist.empty()) {
            cout << "skip " << endl;
            continue;
        }
        string kind_name = board->consist[0].first->kind_desc;
        vector<json> json_arr;
        json info;
        double board_rate = 0;
        board->shape.width += Config::margin * 2;
        board->shape.height += Config::margin * 2;
        for (auto item : board->consist) {
            Copoment *copt = item.first;
            Pos pos = item.second;
            json part;
            part["index"] = copt->id;
            board_rate += copt->getSqrt();
            used_sqrt += copt->getSqrt();
            copt->shape.width -= Config::gap;
            copt->shape.height -= Config::gap;
            part["rect"]["width"] = copt->shape.width;
            part["rect"]["height"] = copt->shape.height;
            part["ratate"] = copt->is_rotated;
            part["startX"] = pos.x + Config::margin;
            part["startY"] = pos.y + Config::margin;
            part["stockNum"] = board->board_id;
            part["priority"] = copt->priority;
            part["cutOrigin"] = "rightTop"; // temp
            {
                json board_json;
                board_json["outHeight"] = board->shape.height;
                board_json["outWidth"] = board->shape.width;
                part["specialInfo"] = board_json;
            }
            json_arr.push_back(part);
        }
        board_rate = (board_rate / (board->shape.getSqrt()));
        tot_sqrt += board->shape.getSqrt();

        info["rate"] = board_rate;
        info["stockNum"] = board->board_id;

        board_infos[kind_name].push_back(info);
        for (const auto& json : json_arr) {
            board_jsons[kind_name].push_back(json);
        }
    }
    final_json["code"] = 1;
    final_json["planUsingTime"] = (clock() - Config::start_time) / 1000;
    final_json["totalBigPlanks"] = 2;
    final_json["totalFanban"] = 0;
    final_json["totalUsedRate"] = used_sqrt / tot_sqrt;
    for (const auto& kv : board_jsons) {
        final_json["resultData"][kv.first]["data"] = kv.second;
        final_json["resultData"][kv.first]["usedRate"] = board_infos[kv.first];
    }
    ofstream file_writer(file_name, ios_base::out);
    file_writer << final_json;
    cout << "total time cost is " << (clock() - Config::start_time) * 1e-6 << endl;
}
inline void inputData() {
    ifstream input(data_path);
    json input_json;
    input >> input_json;
    Config::start_time = clock();
    json maps = input_json["layoutRectMap"];
    json configs = input_json["layoutConfig"];
    Config::gap = configs["gap"];
    Config::margin = configs["margin"];
    Config::height = configs["outHeight"];
    Config::height -= Config::margin * 2;
    Config::width = configs["outWidth"];
    Config::width -= Config::margin * 2;
    Config::cur_dirction = (configs["cutDirection"] != "逆时针");
    vector<string> strs;
    for (auto ths : maps.items()) {
        strs.push_back(ths.key());
    }
    int nums = 0;
    for (auto ths : maps) {
        for (auto item : ths) {
            int idx = item["index"];
            bool roll = item["needRoll"];
            double x = item["rect"]["width"], y = item["rect"]["height"];
            x += Config::gap, y += Config::gap;
            bool rotate = item["rotate"];
            string desc = strs[nums];
            new Copoment(desc, idx, roll, rotate, x, y);
        }
        nums++;
    }
}
void storeDistrubute(distributeMethod &distribute) {
    vector<shared_ptr<Board> > boards;
    int cnt = 0;
    for (const auto &method: distribute.methods) {
        store(method,boards);
        cout << "for " << cnt << ":" << endl;
        method.show();
        cnt++;
    }
    storeBoardsToFile(boards, result_path);
    cout << "the final store value " << distribute.getValue() << endl;
}
inline bool checkRestBlock(int kind_id) {
    double tot_sqrt = 0;
    bool sud = true;
    for (auto bl : Copoment::total_set[kind_id]) {
        if (bl->used == 0) {
            cout << "the " << bl->id << " shp " << bl->shape.height << " " << bl->shape.width << endl;
            tot_sqrt += bl->getSqrt();
            sud = false;
        }
    }
    cout << tot_sqrt << " is sqrt waste " << endl;
    return sud;
}
#endif
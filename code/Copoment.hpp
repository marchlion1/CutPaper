#ifndef COPOMENT_H
#define COPOMENT_H

#include<bits/stdc++.h>

using namespace std;
const double eps = 1e-4;
typedef long long ll;

class Config {
public:
    static int decimal;
    static double gap;
    static double margin;
    static double height, width;
    static bool cur_dirction; // 0 is clockwise, 1 is anti-clock
    static clock_t start_time;

    static double getBoardSqrt();
};

double Config::getBoardSqrt() {
    return Config::height * Config::width;
}

struct Shape {
    double width;
    double height;

    double getSqrt() const;

    inline bool operator==(const Shape &rhs) const {
        return abs(width - rhs.width) < 5 && abs(height - rhs.height) < 5;
    }
};
struct Pos {
    double x, y;

    bool operator==(const Pos &rhs) const {
        return abs(x - rhs.x) < eps && abs(y - rhs.y) < eps;
    }
};
struct line {
    Pos start_point;
    double len;
};
struct space {
    Pos pos;
    Shape shp;
};

class EdgeLines {
public:
    vector<line> contain;

    void show() {
        for (auto li: contain) {
            cout << li.start_point.x << "," << li.start_point.y << " | " << li.len << endl;
        }
    }

    double getHighest() {
        double height = 0;
        for (auto line: contain) {
            height = max(height, line.start_point.y);
        }
        return height;
    }

    space LowestSpace() {
        assert(!contain.empty());

        double height = Config::height;
        double edg = 1e18;
        double len;
        int idx = 0;
        for (int i = 0; i < contain.size(); i++) {
            auto line = contain[i];
            if (line.start_point.y < height) {
                height = line.start_point.y;
                len = line.len;
                idx = i;
            }
        }
        if (idx + 1 < contain.size()) {
            edg = min(edg, contain[idx + 1].start_point.y - height);
        }
        if (idx - 1 >= 0) {
            edg = min(edg, contain[idx - 1].start_point.y - height);
        }
        return space{contain[idx].start_point, {len, edg}};
    }

    bool InsertShape(const Pos &pos, const Shape &shp) {
        vector<line> new_contain;
        bool sud = false;
        for (int i = 0; i < contain.size(); i++) {
            line new_line{};
            if (contain[i].start_point == pos) {
                assert(sud == false);
                sud = true;
                // pos is always one of the line's start point
                new_line.start_point.x = pos.x;
                new_line.start_point.y = pos.y + shp.height;
                new_line.len = shp.width;
                if (i + 1 < contain.size() && pos.x + shp.width > contain[i + 1].start_point.x + 2) {
                    return false;
                }
                contain[i].len -= shp.width;
                contain[i].start_point.x += shp.width;
                size_t idx = new_contain.size();
                if (idx > 0 && abs(new_contain[idx - 1].start_point.y - new_line.start_point.y) < eps) {
                    new_contain[idx - 1].len += new_line.len;
                } else {
                    new_contain.push_back(new_line);
                }
            }
            new_line = contain[i];
            if (new_line.len < eps)
                continue;
            size_t idx = new_contain.size();
            if (idx > 0 && abs(new_contain[(idx - 1)].start_point.y - new_line.start_point.y) < eps) {
                new_contain[idx - 1].len += new_line.len;
            } else {
                new_contain.push_back(new_line);
            }
        }
        assert(sud == true);
        swap(contain, new_contain);
        return sud;
    }


    double getRestSpace() const {
        double rest_space = 0;
        for(auto l:contain){
            rest_space += (Config::height - l.start_point.y ) * l.len;
        }
        return rest_space;
    }
};

class Copoment { // something put into the board
public:
    bool first_use = false;
    static unordered_map<int, vector<Copoment *>> total_set;
    static unordered_map<string, int> kind_id;
    bool is_cut = false;

    Copoment(string &desc, int idx, bool roll, bool rot, double x, double y);

    Shape shape;

    inline double getSqrt() const {
        return shape.height * shape.width;
    }

    string kind_desc;
    int id;
    bool need_roll;
    bool rotate;
    bool is_rotated;
    static int id_card;
    int priority;
    bool used;
};

class Board {
public:
    Board();

    Shape shape{};
    int board_id;
    static int board_id_top;
    vector<pair<Copoment *, Pos>> consist;
    EdgeLines lines;
};

void Rotate(Copoment *block);

unordered_map<int, vector<Copoment *>> Copoment::total_set;
unordered_map<string, int> Copoment::kind_id;
int Copoment::id_card = 1;
double Config::height, Config::width, Config::gap;
int Config::decimal;
bool Config::cur_dirction;
clock_t Config::start_time;
double Config::margin;

Copoment::Copoment(string &desc, int idx, bool roll, bool rot, double x, double y)
        : kind_desc(desc), id(idx), need_roll(roll), rotate(rot), is_rotated(false), shape{x, y} {
    priority = 1;
    if (kind_id[kind_desc] == 0) {
        kind_id[kind_desc] = (id_card++);
    }
    total_set[kind_id[kind_desc]].push_back(this);
    used = false;
}

Board::Board() {
    shape.height = Config::height;
    shape.width = Config::width;
    board_id = board_id_top++;
    lines.contain.push_back({Pos{0, 0}, shape.width});
}

int Board::board_id_top = 1;

void Rotate(Copoment *block) {
    assert(block->rotate == true);
    block->is_rotated ^= 1;
    swap(block->shape.height, block->shape.width);
}

double Shape::getSqrt() const {
    return width * height;
}

double getSumPool(vector<Copoment *> &pool) {
    double tot_sqrt = 0;
    for (auto block: pool) {
        tot_sqrt += block->getSqrt();
    }
    return tot_sqrt;
}
ll getUnusedPoolHash(vector<Copoment *>& pool){
    const ll mod = 1000000000000000003;
    vector<int> ids;
    for(auto block:pool){
        if(block->used)continue;
        ids.push_back(block->id);
    }
    ll hash = 1;
    sort(ids.begin(),ids.end());
    for(int id:ids){
        hash = (hash*131)^id % mod;
    }
    return hash;
}



class CompentKind{
public:
    Shape shape ;
    int id ;

};

#endif
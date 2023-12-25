#ifndef DFSBLOCK
#define DFSBLOCK

#include "Method.hpp"
#include "para.hpp"

int dfs_num = 0;

bool is_fill_all(Place &place, Shape shape) {
    if (place.is_rotated) {
        Rotate(place.block);
    }
    bool sud = (place.block->shape == shape);
    if (place.is_rotated) {
        Rotate(place.block);
    }
    return sud;
}

bool is_equal(Place &lhs, Place &rhs) {
    if (lhs.is_rotated) {
        Rotate(lhs.block);
    }
    if (rhs.is_rotated) {
        Rotate(rhs.block);
    }
    bool sud = lhs.block->shape == rhs.block->shape;
    if (lhs.is_rotated) {
        Rotate(lhs.block);
    }
    if (rhs.is_rotated) {
        Rotate(rhs.block);
    }
    return sud;
}

int getDfsNodeValue(EdgeLines &lines, const DfsPara &para) {
    if (lines.contain.size() == 1) {
        return para.dfs_when_line_1;
    }
    if (lines.contain.size() == 2) {
        return para.dfs_when_line_2;
    }
    return para.dfs_when_line_3;
}

double matchValue(Place &place, Shape &shape) {
    if (place.is_rotated) {
        Rotate(place.block);
    }
    double d1 = abs(place.block->shape.width - shape.width);
    double d2 = abs(place.block->shape.height - shape.height);
    if (place.is_rotated) {
        Rotate(place.block);
    }
    return min(d1, d2) / (max(d1, d2) + eps);
}

bool cmpByValue(const Place &lhs, const Place &rhs) {
    if (lhs.block->first_use == rhs.block->first_use) {
        return lhs.value < rhs.value;
    }
    return lhs.block->first_use > rhs.block->first_use;
}

vector<Place> getBestPlace(const vector<Copoment *> &pool, Shape &shape, Pos &pos) {
    vector<Place> places;
    for (auto block: pool) {
        if (block->used)
            continue;
        assert(block->is_rotated == false);
        if (block->shape.width < shape.width + eps && block->shape.height < Config::height - (pos.y) + eps) {
            places.push_back(Place{pos, block, false});
        }
        if (block->rotate && block->shape.height < shape.width + eps &&
            block->shape.width < Config::height - (pos.y) + eps) {
            places.push_back(Place{pos, block, true});
        }
    }
    for (auto &place: places) {
        place.value = matchValue(place, shape);
    }
    sort(places.begin(), places.end(), cmpByValue);
    vector<Place> new_places;
    for (auto place: places) {
        if (!new_places.empty() && is_equal(new_places[new_places.size() - 1], place)) {
            continue;
        }
        new_places.push_back(place);
    }
    return new_places;
}

bool isAllUsed(const vector<Copoment *> &pool) {
    auto isUsed = [](Copoment *block) {
        return block->used;
    };
    return std::all_of(pool.begin(), pool.end(), isUsed);
}

int dfsMethod(vector<Copoment *> &pool, EdgeLines lines, Method &way, const DfsPara &para, int deep = 0) {
    dfs_num++;

    if (lines.getHighest() > Config::height)
        return 0;
    //TODO debug the waste area ( tot_sqrt + waste < board_sqrt)
    int update_num = 0;
    update_num += way.update(way.cur_sqrt, lines.getRestSpace());
    if (deep == pool.size()) {
        way.is_best = true;
        return update_num;
    }
    if (deep == para.max_dfs_deep) {
        return update_num;
    }
    space low_part = lines.LowestSpace();
    int max_dfs_node = getDfsNodeValue(lines, para);
    if (deep >= para.limit_dfs_deep) {
        max_dfs_node = 1;
    }
    vector<Place> places = getBestPlace(pool, low_part.shp, low_part.pos);
    if (places.empty()) {
        assert(lines.InsertShape(low_part.pos, low_part.shp));
        way.cur_waste += low_part.shp.getSqrt();
        dfsMethod(pool, lines, way, para, deep);
        way.cur_waste -= low_part.shp.getSqrt();
    } else {
        int extern_search_node = 0;//(deep<5);
        for (auto &place: places) {
            if (update_num >= 1) {
                extern_search_node = 1;
            }
            if (update_num >= 5) {
                extern_search_node = 2;
            }
            if ((max_dfs_node--) + extern_search_node <= 0)break;
            if (place.is_rotated) {
                Rotate(place.block);
            }
            EdgeLines new_line = lines;
            assert(new_line.InsertShape(low_part.pos, place.block->shape));
            way.arange.push_back(place);
            place.block->used = true;
            way.cur_sqrt += place.block->getSqrt();
            update_num += dfsMethod(pool, new_line, way, para, deep + 1);
            way.arange.pop_back();
            way.cur_sqrt -= place.block->getSqrt();
            place.block->used = false;
            if (place.is_rotated) {
                Rotate(place.block);
            }
            if (way.is_best)break;
        }
    }
    return update_num;
}

Method getDfsMethod(vector<Copoment *> &pool, const DfsPara &para) {
    static unordered_map<ll, Method> hash_methods;
    ll hash = getUnusedPoolHash(pool);
    //cout << "for a dfs process " << endl;
    if (hash_methods.find(hash) != hash_methods.end()) {
        // cout << "cache here " << endl;
        return hash_methods[hash];
    }
    dfs_num = 0;
    Board board;
    Method method;
    dfsMethod(pool, board.lines, method, para);
    hash_methods[hash] = method;
    //cout << "for dfs num is " << dfs_num << " siz " << pool.size() << " and best size " << method.best.size() << endl;
    return method;
}

#endif
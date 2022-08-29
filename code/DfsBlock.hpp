#ifndef DFSBLOCK
#define DFSBLOCK
#include "Method.hpp"

ll getUnusedPoolHash(vector<Copoment *>& pool){
    const int mod = 1e9+7;
    ll hash = 1;
    for(auto block:pool){
        if(block->used)continue;
        hash = (hash * block->id) % mod;
    }
    return hash;
}

bool cmp(const Place &lhs, const Place &rhs) {
    if (lhs.block->first_use == rhs.block->first_use) {
        return lhs.value < rhs.value;
    }
    return lhs.block->first_use > rhs.block->first_use;
}
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
    return lhs.is_rotated == rhs.is_rotated && lhs.block->shape == rhs.block->shape;
}
int getDfsNodeValue(EdgeLines &lines, Method &way) {
    if (lines.contain.size() == 1) {
        return 5;
    }
    if (lines.contain.size() == 2) {
        return 3;
    }
    return 1;
}
double min_dis(Place& place,Shape& shape){
    if(place.is_rotated){
        Rotate(place.block);
    }
    double d1 = abs(place.block->shape.width - shape.width);
    double d2 = abs(place.block->shape.height - shape.height);
    if(place.is_rotated){
        Rotate(place.block);
    }
    return min(d1,d2) / (max(d1,d2)+0.01);
}
void setPlaceValue(Place& place,Shape& shape){
    place.value = min_dis(place,shape);
}
vector<Place> getBestPlace(const vector<Copoment *> &pool, Shape &shape, Pos &pos, int size) {
    vector<Place> places;
    for (auto block: pool) {
        if (block->used)
            continue;
        // TODO refine the choose method
        if (block->shape.width < shape.width + eps && block->shape.height < Config::height - (pos.y) + eps) {
            places.push_back(Place{pos, block, false});
        }
        if (block->rotate && block->shape.height < shape.width + eps &&
            block->shape.width < Config::height - (pos.y) + eps) {
            places.push_back(Place{pos, block, true});
        }
    }

    for (auto &place: places) {
        setPlaceValue(place, shape);
    }
    sort(places.begin(), places.end(), cmp);
    vector<Place> new_places;
    bool fill_all = false;
    for (auto place: places) {
        if (!new_places.empty() && is_equal(new_places[new_places.size() - 1], place)) {
            continue;
        }
        if (new_places.size() >= size)
            continue;

        if (fill_all)
            break;

        if (is_fill_all(place, shape)) {
            fill_all = true;
        }
        new_places.push_back(place);
    }
    return new_places;
}
void dfsMethod(vector<Copoment *> &pool, EdgeLines lines, Method &way, int deep = 0) {
    if (lines.getHighest() > Config::height)
        return;
    way.update(way.cur_sqrt);
    if (deep == 16)
        return;
    space low_part = lines.LowestSpace();
    int dfs_num = getDfsNodeValue(lines, way);
    vector<Place> places = getBestPlace(pool, low_part.shp, low_part.pos, dfs_num);
    if (places.empty()) {
        lines.InsertShape(low_part.pos, low_part.shp);
        way.cur_waste += low_part.shp.getSqrt();
        dfsMethod(pool, lines, way, deep);
        way.cur_waste -= low_part.shp.getSqrt();
    } else {
        for (auto &place: places) {
            if (place.is_rotated) {
                Rotate(place.block);
            }
            EdgeLines new_line = lines;
            new_line.InsertShape(low_part.pos, place.block->shape);
            way.arange.push_back(place);
            place.block->used = true;
            way.cur_sqrt += place.block->getSqrt();
            dfsMethod(pool, new_line, way, deep + 1);
            way.arange.pop_back();
            way.cur_sqrt -= place.block->getSqrt();
            place.block->used = false;
            if (place.is_rotated) {
                Rotate(place.block);
            }
        }
    }
}

Method getDfsMethod(vector<Copoment *> &pool) {
    static unordered_map<ll,Method> hash_methods;
    ll hash = getUnusedPoolHash(pool);
    if(hash_methods.find(hash) != hash_methods.end()){
        return hash_methods[hash];
    }
    Board board;
    Method method;
    dfsMethod(pool, board.lines, method);
    hash_methods[hash] = method;
    return method;
}

#endif
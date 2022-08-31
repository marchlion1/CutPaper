#ifndef DFSBLOCK
#define DFSBLOCK
#include "Method.hpp"

ll getUnusedPoolHash(vector<Copoment *>& pool){
    const ll mod = 1e9+7;
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
int getDfsNodeValue(EdgeLines &lines) {
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
    return min(d1,d2) / (max(d1,d2)+eps);
}
void setPlaceValue(Place& place,Shape& shape){
    place.value = min_dis(place,shape);
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
        setPlaceValue(place, shape);
    }
    sort(places.begin(), places.end(), cmp);
    vector<Place> new_places;
    for (auto place: places) {
        if (!new_places.empty() && is_equal(new_places[new_places.size() - 1], place)) {
            continue;
        }
        new_places.push_back(place);
    }
    return new_places;
}
bool isAllUsed(vector<Copoment*>& pool){
    for(auto block:pool){
        if(!block->used)return false;
    }
    return true;
}
int dfsMethod(vector<Copoment *> &pool, EdgeLines lines, Method &way, int deep = 0) {
    if (lines.getHighest() > Config::height)
        return 0;
    /*
    double tot_sqrt = 0;
    for(auto place:way.arange){
        tot_sqrt += place.block->getSqrt();
    }
    double board_sqrt = Config::height * Config::width;
     */
    //TODO debug the waste area ( tot_sqrt + waste < board_sqrt)
    int update_num = 0;
    update_num += way.update(way.cur_sqrt);
    if(isAllUsed(pool)){
        way.is_best = true;
        return update_num;
    }
    if (deep == 16){
        return update_num;
    }
    space low_part = lines.LowestSpace();
    int max_dfs_node = getDfsNodeValue(lines);
    if(deep >= 10){
        max_dfs_node = 1;
    }
    vector<Place> places = getBestPlace(pool, low_part.shp, low_part.pos);
    if (places.empty()) {
        assert(lines.InsertShape(low_part.pos, low_part.shp) );
        way.cur_waste += low_part.shp.getSqrt();
        dfsMethod(pool, lines, way, deep);
        way.cur_waste -= low_part.shp.getSqrt();
    } else {
        int extern_search_node = 0;//(deep<5);
        for (auto &place: places) {
            if(update_num >= 1){
                extern_search_node = 1;
            }
            if(update_num >= 5) {
                extern_search_node = 2;
            }
            if((max_dfs_node--)+extern_search_node <= 0)break;
            if (place.is_rotated) {
                Rotate(place.block);
            }
            EdgeLines new_line = lines;
            assert( new_line.InsertShape(low_part.pos, place.block->shape));
            way.arange.push_back(place);
            place.block->used = true;
            way.cur_sqrt += place.block->getSqrt();
            update_num+=dfsMethod(pool, new_line, way, deep + 1);
            way.arange.pop_back();
            way.cur_sqrt -= place.block->getSqrt();
            place.block->used = false;
            if (place.is_rotated) {
                Rotate(place.block);
            }
            if(way.is_best)break;
        }
    }
    return update_num;
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
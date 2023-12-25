#include <memory>

#ifndef METHOD
#define METHOD
struct Place {
    Pos pos;
    Copoment *block;
    bool is_rotated;
    double value;
};

struct Method {
    vector<Place> arange;
    vector<Place> best;
    double cur_waste = 0;
    double max_sqrt = 0;
    double max_rest = 0;
    double cur_sqrt = 0;
    bool is_best = false;

    bool update(double sqrt, double rest_space) {
        if (sqrt + rest_space * 0.1 < max_sqrt + max_rest * 0.1 + 10)
            return false;
        max_sqrt = sqrt;
        max_rest = rest_space;
        best = arange;
        return true;
    }

    double getUseRate() const {
        return getSqrt() / (Config::width * Config::height);
    }
    double getSorce() const{
        return (getSqrt() + max_rest * 0.1) / Config::getBoardSqrt();
    }
    double getSqrt() const {
        return max_sqrt;
    }

    void show() const {
        cout << "method use rate " << getUseRate() << " use " << getSqrt() << endl;

        for (auto place: best) {
            cout << " " << place.block->id;//<<" in "<< place.pos.x<<","<<place.pos.y;
        }
        cout << "-----\n";
    }
};

void store(const Method &method, vector<shared_ptr<Board>> &boards) {
    auto board = std::make_shared<Board>();
    for (auto place: method.best) {
        if (place.is_rotated && !place.block->is_rotated) {
            Rotate(place.block);
        }
        board->consist.emplace_back(place.block, place.pos);
        place.block->used = true;
    }
    boards.push_back(board);
}


#endif
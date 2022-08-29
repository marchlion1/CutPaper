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
    vector<int> rot_record;
    double cur_waste = 0;
    double max_sqrt = 0;
    double cur_sqrt = 0;

    void update(double sqrt) {
        if (sqrt < max_sqrt + 1)
            return;
        max_sqrt = sqrt;
        best = arange;
    }

    double getUseRate() const {
        return getSqrt() / (Config::width * Config::height);
    }

    double getSqrt() const {
        double sqrt = 0;
        for (auto place: best) {
            sqrt += place.block->getSqrt();
        }
        return sqrt;
    }
};


#endif
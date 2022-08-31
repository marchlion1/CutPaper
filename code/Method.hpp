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
    double cur_sqrt = 0;
    bool is_best = false;

    bool update(double sqrt) {
        if (sqrt < max_sqrt + 10)
            return false;
        max_sqrt = sqrt;
        best = arange;
        return true;
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

    void show() const {
        cout <<"method "<< getUseRate() << endl;
        for(auto place:best){
            cout <<" "<<place.block->id ;//<<" in "<< place.pos.x<<","<<place.pos.y;
        }
        cout <<"-----\n";
    }
};


#endif
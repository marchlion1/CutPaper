#ifndef SKL
#define SKL

#include "Copoment.hpp"
#include <set>

struct seg {
    double x, y;
    double len;
};

class SKLines {
public:
    list<seg> S;
    double max_spread, max_height;
    vector<Copoment *> CopomentSet;
    multiset<int> widths;

    void execPlace(int index, int dir, Copoment *block);

private:
    double updateWaste();

    void execRoll();

    void execDirRoll(bool dir);
};
void SKLines::execPlace(int index, int dir, Copoment *block) {
    assert(index >= 0 && index < S.size());
    assert(dir == 0 || dir == 1);
    double width = block->shape.width, height = block->shape.height;
    int i = 0;
    for (auto iter = S.begin(); iter != S.end(); i++) {
        if (i == index) {
            assert((*iter).y + height < max_height);
            seg add_line;
            add_line.x = (dir == 0 ? (*iter).x : (*iter).x + (*iter).len - (width));
            add_line.y = (*iter).y + height;
            add_line.len = width;
            S.insert(iter, add_line);
            return;
        }
        ++iter;
    }
}

void SKLines::execRoll() {
    reverse(S.begin(), S.end());
    execDirRoll(true);
    reverse(S.begin(), S.end());
    execDirRoll(false);
    auto iter = S.begin();
    int min_width = (*widths.begin());
    while (S.size() > 1) {
        bool left = false;
        double lefy_y = 1e9, right_y = 1e9;
        auto riter = iter;
        auto liter = iter;
        if (iter == S.begin()) {
            left = true;
        } else {
            --liter;
            left = (*liter).y > (*iter).y;
            lefy_y = (*liter).y;
        }
        bool right = false;
        if (iter == (--S.end())) {
            right = true;
        } else {
            ++riter;
            right = (*riter).y > (*iter).y;
            right_y = (*riter).y;
        }
        if (right && left && (*iter).len < min_width) {
            if (abs(lefy_y - right_y) < eps){
                (*liter).len += (*iter).len + (*riter).len;
                iter = S.erase(iter, ++riter);
            }
            else if (right_y < lefy_y) {
                (*riter).x = (*iter).x;
                (*riter).len += (*iter).len;
                iter= S.erase(iter);
            } else {
                (*liter).len += (*iter).len;
                iter= S.erase(iter);
                --iter;
            }
        } else {
            ++iter;
        }
    }
}

void SKLines::execDirRoll(bool dir) {
    auto iter = S.begin();
    seg maxLine{};
    maxLine.x = -1;
    maxLine.y = -1;
    maxLine.len = 0;
    while (iter != S.end()) {
        if ((*iter).y > maxLine.y) {
            maxLine = (*iter);
            ++iter;
            continue;
        }
        if (!dir) {
            if ((*iter).x < maxLine.x + maxLine.len)
                if ((*iter).x + (*iter).len > maxLine.x + maxLine.len) {
                    (*iter).x = maxLine.x + maxLine.len;
                    maxLine = (*iter);
                    ++iter;
                } else {
                    iter = S.erase(iter);
                }
            else {
                maxLine = (*iter);
                ++iter;
            }
        } else {
            if ((*iter).x + (*iter).len > maxLine.x) {
                if ((*iter).x + (*iter).len < maxLine.x + maxLine.len) {
                    (*iter).len = (maxLine.x - (*iter).x);
                    maxLine = (*iter);
                    ++iter;
                } else {
                    iter = S.erase(iter);
                }
            } else {
                maxLine = (*iter);
                ++iter;
            }
        }
    }
}

#endif SKL
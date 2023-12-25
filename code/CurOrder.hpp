#ifndef CURODER
#define CURODER
const int UP = 1, DOWN = -1, LEFT = 2, RIGHT = -2;
struct edge {
    int to;
    double value;
    int dir;
};

double cross(double x1, double y1, double x2, double y2) {
    if (x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }
    if (y1 < x2) {
        return 0;
    }
    return min(y2, y1) - x2;
}

edge getCrossCopoment(int i, int j, const Method &method) {
    Place A = method.best[i], B = method.best[j];
    // i is left by j
    if (A.is_rotated) {
        Rotate(A.block);
    }
    if (B.is_rotated) {
        Rotate(B.block);
    }
    edge ans;
    if (abs(A.pos.x - (B.pos.x + B.block->shape.width) < eps)) {
        double C = cross(A.pos.y, A.pos.y + A.block->shape.height, B.pos.y, B.pos.y + B.block->shape.height);
        if (C > eps){
                ans = edge{ i, C, LEFT };
        }
    }// left
    if (abs(B.pos.x - (A.pos.x + A.block->shape.width) < eps)) {
        double C = cross(A.pos.y, A.pos.y + A.block->shape.height, B.pos.y, B.pos.y + B.block->shape.height);
        if (C > eps){
            ans = edge{ i, C, RIGHT};
        }
    }// right

    if(abs ((A.pos.y + A.block->shape.height)-B.pos.y) < eps ) {
        double C = cross(A.pos.x , A.pos.x + A.block->shape.width, B.pos.x, B.pos.x + B.block->shape.width ) ;
        if(C > eps){
            ans = edge{i,C,UP};
        }
    }//UP
    if(abs ((B.pos.y + B.block->shape.height)-A.pos.y) < eps ) {
        double C = cross(A.pos.x , A.pos.x + A.block->shape.width, B.pos.x, B.pos.x + B.block->shape.width ) ;
        if(C > eps){
            ans = edge{i,C, DOWN};
        }
    }//DOWN
    if (A.is_rotated) {
        Rotate(A.block);
    }
    if (B.is_rotated) {
        Rotate(B.block);
    }
    return ans;
}

class Graph {
public:
    vector<vector<edge>> edges;
    vector<Copoment *> copoments;
    vector<double> values;

    void init(vector<Copoment *> &pool) {
        copoments = pool;
        edges.resize(pool.size());
        values.resize(pool.size());
        fill(values.begin(), values.end(), 0);
    }

    void insertEdge(int i, int j, double value) {
        assert(i >= 0 && i < copoments.size());
        assert(j >= 0 && j < copoments.size());
        edges[i].push_back({j, value});
        edges[j].push_back({i, value});
        values[i] += value;
        values[j] += value;
    }

    void solve() {
        // TODO optimizate the algorithm
        int cnt = 1;
        while (cnt <= copoments.size()) {
            double min_value = 1e9;
            int idx = 0;
            for (int i = 0; i < values.size(); i++) {
                if (values[i] < min_value) {
                    min_value = values[i];
                    idx = i;
                }
            }
            copoments[idx]->priority = cnt;
            copoments[idx]->is_cut = true;
            cnt++;
            values[idx] = 1e9;
            for (auto e: edges[idx]) {
                values[e.to] -= e.value;
            }
        }
    }
};

double getLapLength(Place lhs, Place rhs) {
    if (rhs.pos.x < lhs.pos.x)
        swap(rhs, lhs);
    if (rhs.is_rotated) {
        Rotate(rhs.block);
    }
    if (lhs.is_rotated) {
        Rotate(lhs.block);
    }
    double laplen = 0;
    if (abs(lhs.block->shape.width + lhs.pos.x - rhs.pos.x) < eps) {
        if (rhs.pos.y <= lhs.pos.y + lhs.block->shape.height && rhs.pos.y >= lhs.pos.y) {
            laplen = max(laplen, min(lhs.pos.y + lhs.block->shape.height - rhs.pos.y, rhs.block->shape.height));
        }
    }
    if (abs(lhs.pos.y - (rhs.pos.y + rhs.block->shape.height)) < eps ||
        abs(lhs.pos.y + lhs.block->shape.height - (rhs.pos.y)) < eps) {
        if (rhs.pos.x <= lhs.pos.x + lhs.block->shape.width) {
            laplen = max(laplen, min(lhs.pos.x + lhs.block->shape.width - rhs.pos.x, rhs.block->shape.width));
        }
    }
    if (lhs.is_rotated) {
        Rotate(lhs.block);
    }
    if (rhs.is_rotated) {
        Rotate(rhs.block);
    }

    return laplen;
}

void setPriority(Method &method) {
    vector<Copoment *> pool;
    for (auto place: method.best) {
        pool.push_back(place.block);
    }
    Graph abstract_graph;
    abstract_graph.init(pool);
    for (int i = 0; i < method.best.size(); i++) {
        for (int j = i + 1; j < method.best.size(); j++) {
            double laplen = getLapLength(method.best[i], method.best[j]);
            if (laplen < eps)
                continue;
            abstract_graph.insertEdge(i, j, laplen);
        }
    }
    abstract_graph.solve();
}

double getConnectPartSqrt(Graph &G, int start_p, int &mask, const vector<Copoment *> &pool) {
    assert(start_p < pool.size());
    if ((mask >> start_p) & 1) return 0;
    double tot_sqrt = pool[start_p]->getSqrt();
    for (auto [to, value, dir]: G.edges[start_p]) {
        tot_sqrt += getConnectPartSqrt(G, to, mask, pool);
    }
    return tot_sqrt;
}


#endif
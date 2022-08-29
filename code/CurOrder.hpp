#ifndef CURODER
#define CURODER
struct edge {
    int to;
    double value;
};
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

void setpriority(Method &method) {
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
#endif
#ifndef BRANCH_BOUND_H
#define BRANCH_BOUND_H

#include "Knapsack.h"
#include <vector>
#include <queue>

class branch_and_bound : public Knapsack {
private:
    std::vector<int> sorted_indices;
    
    struct Node {
        int level;
        int profit;
        int weight;
        double bound;
        std::vector<int> taken;
        
        Node(int lvl, int prof, int w, double b, const std::vector<int>& t)
            : level(lvl), profit(prof), weight(w), bound(b), taken(t) {}
    };
    
    struct compare_node {
        bool operator()(const Node& a, const Node& b) {
            return a.bound < b.bound;
        }
    };
    
    void sort_items_by_ratio();
    double calculate_bound(int level, int profit, int weight) const;
    
public:
    branch_and_bound(int n, int w, const std::vector<Item>& its);
    
    virtual std::vector<int> get_zero_solution() const override;
    virtual std::vector<int> get_neigbour_solution(const std::vector<int>& current) const override;
};

#endif

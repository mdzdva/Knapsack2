#ifndef KNAPSACK_H
#define KNAPSACK_H

#include "Task.h"
#include "Item.h"
#include <vector>
#include <random>

class Knapsack: public Task {
protected:
    int N; //number of items
    int W; //how much fits in the knapsack
    std::vector<Item> items;
    mutable std::mt19937 rg; //generator of random numbers
    mutable std::uniform_int_distribution<int> dist; //for hamming
    
public:
    Knapsack(int n, int w, const std::vector<Item>& its);
    
    std::vector<int> get_zero_solution() const override;
    std::vector<int> get_neigbour_solution(const std::vector<int>& current) const override;
    int how_good_is_solution(const std::vector<int>& sol) const override;
    bool is_correct_solution(const std::vector<int>& sol) const override;
    
    int summary_weight(const std::vector<int>& sol) const;
    void print(const std::vector<int>& sol) const;
};

#endif

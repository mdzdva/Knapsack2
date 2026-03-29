#include "branch_and_bound.h"
#include <iostream>
#include <queue>
#include <algorithm>

branch_and_bound::branch_and_bound(int n, int w, const std::vector<Item>& its): Knapsack(n, w, its) {
    sort_items_by_ratio();
}

void branch_and_bound::sort_items_by_ratio() {
    sorted_indices.resize(N);
    for(int i = 0; i < N; i++) {
        sorted_indices[i] = i;
    }
    
    std::sort(sorted_indices.begin(), sorted_indices.end(), [this](int a, int b) {
        double ratio_a = (items[a].weight == 0) ? 1e9 : static_cast<double>(items[a].value) / items[a].weight;
        double ratio_b = (items[b].weight == 0) ? 1e9 : static_cast<double>(items[b].value) / items[b].weight;
        return ratio_a > ratio_b;
    });
}

double branch_and_bound::calculate_bound(int level, int profit, int weight) const {
    if(weight >= W) return 0;
    
    double bound = profit;
    int current_weight = weight;
    
    for(int i = level; i < N; i++) {
        if(current_weight >= W) {
            i = N;
        } else {
            int idx = sorted_indices[i];
            if(current_weight + items[idx].weight <= W) {
                bound += items[idx].value;
                current_weight += items[idx].weight;
            } else {
                int remaining = W - current_weight;
                bound += items[idx].value * (static_cast<double>(remaining) / items[idx].weight);
                i = N;
            }
        }
    }
    
    return bound;
}

std::vector<int> branch_and_bound::get_zero_solution() const {
    std::vector<int> solution(N, 0);
    int current_weight = 0;
    
    for(int i = 0; i < N; i++) {
        int idx = sorted_indices[i];
        if(current_weight + items[idx].weight <= W) {
            solution[idx] = 1;
            current_weight += items[idx].weight;
        }
    }
    
    return solution;
}

std::vector<int> branch_and_bound::get_neigbour_solution(const std::vector<int>& current) const {
    std::priority_queue<Node, std::vector<Node>, compare_node> pq;
    
    std::vector<int> best_solution = get_zero_solution();
    int best_profit = how_good_is_solution(best_solution);
    
    std::vector<int> initial_taken(N, 0);
    double initial_bound = calculate_bound(0, 0, 0);
    Node root(-1, 0, 0, initial_bound, initial_taken);
    pq.push(root);
    
    while(!pq.empty()) {
        Node current_node = pq.top();
        pq.pop();
        
        if(!(current_node.bound <= best_profit)) {
            int next_level = current_node.level + 1;
            if(next_level < N) {
                int idx = sorted_indices[next_level];
                
                std::vector<int> not_taken = current_node.taken;
                double bound_not_take = calculate_bound(next_level + 1,
                                                       current_node.profit,
                                                       current_node.weight);
                
                if(bound_not_take > best_profit) {
                    Node child_not_take(next_level, current_node.profit,
                                       current_node.weight, bound_not_take, not_taken);
                    pq.push(child_not_take);
                }
                
                if(current_node.weight + items[idx].weight <= W) {
                    std::vector<int> taken = current_node.taken;
                    taken[idx] = 1;
                    int new_profit = current_node.profit + items[idx].value;
                    int new_weight = current_node.weight + items[idx].weight;
                    
                    if(new_profit > best_profit) {
                        best_profit = new_profit;
                        best_solution = taken;
                    }
                    
                    double bound_take = calculate_bound(next_level + 1, new_profit, new_weight);
                    
                    if(bound_take > best_profit) {
                        Node child_take(next_level, new_profit, new_weight, bound_take, taken);
                        pq.push(child_take);
                    }
                }
            }
        }
    }
    
    return best_solution;
}

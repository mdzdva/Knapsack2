#include "Knapsack.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Knapsack::Knapsack(int n, int w, const std::vector<Item>& its): N(n), W(w), items(its), rg(std::random_device()()), dist(0, N-1) {
    std::random_device r;
    rg.seed(r());
}

bool Knapsack::is_correct_solution(const std::vector<int> &sol) const {
    int summary_weight = 0;
    for(int i = 0; i<N; i++) {
        if(sol[i] == 1) {
            summary_weight += items[i].weight;
        }
    }
    return summary_weight <= W;
}

int Knapsack::how_good_is_solution(const std::vector<int> &sol) const {
    int summary_value = 0;
    for(int i = 0; i<N; i++) {
        if(sol[i] ==1 ) {
            summary_value += items[i].value;
        }
    }
    return summary_value;
}

int Knapsack::summary_weight(const std::vector<int> &sol) const {
    int weight = 0;
    for(int i = 0; i<N; i++) {
        if(sol[i] == 1) {
            weight += items[i].weight;
        }
    }
    return weight;
}

void Knapsack::print(const std::vector<int> &sol) const {
    int weight = summary_weight(sol);
    std::cout << weight << '\n';
    
    for(int i = 0; i<N; i++) {
        std::cout << sol[i];
        if(i<N-1) std::cout << ' ';
    }
    std::cout << '\n';
}

std::vector<int> Knapsack::get_zero_solution() const {
    std::vector<int> indices(N);
    for(int i = 0; i<N; i++) {
        indices[i] = i;
    }
    
    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        double ratio_a;
        if(items[a].weight == 0) {
            ratio_a = 1e7;
        } else {
            ratio_a = static_cast<double>(items[a].value)/items[a].weight;
        }
        
        double ratio_b;
        if(items[b].weight == 0) {
            ratio_b = 1e7;
        } else {
            ratio_b = static_cast<double>(items[b].value)/items[b].weight;
        }
        return ratio_a>ratio_b;
    });
    
    std::vector<int> solution(N, 0);
    int current_weight = 0;
    for(int i = 0; i<N; i++) {
        int idx = indices[i];
        if(current_weight + items[idx].weight<=W) {
            solution[idx] = 1;
            current_weight += items[idx].weight;
        }
    }
    return solution;
}

std::vector<int> Knapsack::get_neigbour_solution(const std::vector<int>& current) const {
    std::vector<int> neighbour = current;
    
    std::uniform_int_distribution<int> hamming_dist(1, 3);
    int k = hamming_dist(rg);
    
    for(int change = 0; change < k; ++change) {
        std::uniform_int_distribution<int> idx_dist(0, N - 1);
        int flip_index = idx_dist(rg);
        neighbour[flip_index] = 1 - neighbour[flip_index];
    }
    
    if(!is_correct_solution(neighbour)) {
        bool feasible = false;
        
        for(int attempt = 0; attempt < 5 && !feasible; ++attempt) {
            std::vector<int> temp = neighbour;
            
            std::vector<int> taken;
            for (int i = 0; i < N; ++i) {
                if (temp[i] == 1) {
                    taken.push_back(i);
                }
            }
            
            for(int remove = 0; remove < k && !taken.empty(); ++remove) {
                std::uniform_int_distribution<int> taken_dist(0, taken.size() - 1);
                int idx_to_remove = taken_dist(rg);
                temp[taken[idx_to_remove]] = 0;
                taken.erase(taken.begin() + idx_to_remove);
            }
            
            if(is_correct_solution(temp)) {
                neighbour = temp;
                feasible = true;
            }
        }
        
        if(!feasible) {
            return current;
        }
    }
    
    return neighbour;
}

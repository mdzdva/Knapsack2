#include "branch_and_bound.h"
#include <iostream>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    
    int N, W;
    std::cin >> N >> W;
    
    std::vector<Item> items(N);
    for(int i = 0; i < N; i++) {
        std::cin >> items[i].value >> items[i].weight;
    }
    
    branch_and_bound knapsack(N, W, items);
    
    std::vector<int> solution = knapsack.get_zero_solution();
    
    knapsack.print(solution);
    
    return 0;
}

#include "Genetic.h"
#include <iostream>
#include <algorithm>

Genetic::Genetic(int n, int w, const std::vector<Item>& its, int pop_size, double mut_rate, double cross_rate, int max_gen): Knapsack(n, w, its), population_size(pop_size), mutation_rate(mut_rate), crossover_rate(cross_rate), max_generations(max_gen) {
    std::random_device rd;
    rg.seed(rd());
}

void Genetic::calculate_fitness(Individual& ind) const {
    ind.weight = 0;
    ind.fitness = 0;
    
    for(int i = 0; i < N; i++) {
        if(ind.genes[i] == 1) {
            ind.weight += items[i].weight;
            ind.fitness += items[i].value;
        }
    }
    
    if(ind.weight > W) {
        ind.fitness = 0;
    }
}

void Genetic::repair_solution(std::vector<int>& solution) const {
    std::vector<int> indices(N);
    for(int i = 0; i < N; i++) {
        indices[i] = i;
    }
    
    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        return items[a].value > items[b].value;
    });
    
    int current_weight = 0;
    for(int i = 0; i < N; i++) {
        if(solution[i] == 1) {
            current_weight += items[i].weight;
        }
    }
    
    if(current_weight > W) {
        std::vector<std::pair<double, int> > items_to_remove;
        for(int i = 0; i < N; i++) {
            if(solution[i] == 1) {
                double ratio = static_cast<double>(items[i].value) / items[i].weight;
                items_to_remove.push_back(std::make_pair(ratio, i));
            }
        }
        
        std::sort(items_to_remove.begin(), items_to_remove.end());
        
        for(size_t j = 0; j < items_to_remove.size(); j++) {
            if(current_weight <= W) {
                j = items_to_remove.size();
            } else {
                int item_idx = items_to_remove[j].second;
                solution[item_idx] = 0;
                current_weight -= items[item_idx].weight;
            }
        }
    }
    
    if(current_weight < W) {
        std::vector<std::pair<double, int> > items_to_add;
        for(int i = 0; i < N; i++) {
            if(solution[i] == 0) {
                double ratio = static_cast<double>(items[i].value) / items[i].weight;
                items_to_add.push_back(std::make_pair(ratio, i));
            }
        }
        
        std::sort(items_to_add.begin(), items_to_add.end(),
                 [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                     return a.first > b.first;
                 });
        
        for(size_t j = 0; j < items_to_add.size(); j++) {
            int item_idx = items_to_add[j].second;
            if(current_weight + items[item_idx].weight <= W) {
                solution[item_idx] = 1;
                current_weight += items[item_idx].weight;
            }
        }
    }
}

Genetic::Individual Genetic::create_random_individual() const {
    Individual ind(N);
    std::uniform_int_distribution<int> dist(0, 1);
    
    for(int i = 0; i < N; i++) {
        ind.genes[i] = dist(rg);
    }
    
    repair_solution(ind.genes);
    calculate_fitness(ind);
    return ind;
}

Genetic::Individual Genetic::crossover(const Individual& parent1, const Individual& parent2) const {
    Individual child(N);
    std::uniform_int_distribution<int> dist(0, 1);
    std::uniform_int_distribution<int> point_dist(1, N-1);
    
    if(dist(rg) < crossover_rate) {
        int crossover_point = point_dist(rg);
        
        for(int i = 0; i < crossover_point; i++) {
            child.genes[i] = parent1.genes[i];
        }
        for(int i = crossover_point; i < N; i++) {
            child.genes[i] = parent2.genes[i];
        }
    } else {
        child.genes = parent1.genes;
    }
    
    repair_solution(child.genes);
    calculate_fitness(child);
    return child;
}

void Genetic::mutate(Individual& ind) const {
    std::uniform_int_distribution<int> idx_dist(0, N-1);
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    
    for(int i = 0; i < N; i++) {
        if(prob_dist(rg) < mutation_rate) {
            ind.genes[i] = 1 - ind.genes[i];
        }
    }
    
    repair_solution(ind.genes);
    calculate_fitness(ind);
}

Genetic::Individual Genetic::tournament_selection(
    const std::vector<Individual>& population) const {
    std::uniform_int_distribution<int> dist(0, population.size() - 1);
    int tournament_size = 3;
    int best_idx = dist(rg);
    
    for(int i = 1; i < tournament_size; i++) {
        int idx = dist(rg);
        if(population[idx].fitness > population[best_idx].fitness) {
            best_idx = idx;
        }
    }
    
    return population[best_idx];
}

std::vector<int> Genetic::get_zero_solution() const {
    std::vector<Individual> population;
    
    std::vector<int> greedy_solution(N, 0);
    int current_weight = 0;
    
    std::vector<int> indices(N);
    for(int i = 0; i < N; i++) indices[i] = i;
    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        double ratio_a = (items[a].weight == 0) ? 1e9 : static_cast<double>(items[a].value) / items[a].weight;
        double ratio_b = (items[b].weight == 0) ? 1e9 : static_cast<double>(items[b].value) / items[b].weight;
        return ratio_a > ratio_b;
    });
    
    for(int i = 0; i < N; i++) {
        int idx = indices[i];
        if(current_weight + items[idx].weight <= W) {
            greedy_solution[idx] = 1;
            current_weight += items[idx].weight;
        }
    }
    
    Individual greedy_ind(greedy_solution, 0, 0);
    calculate_fitness(greedy_ind);
    population.push_back(greedy_ind);
    
    for(int i = 1; i < population_size; i++) {
        population.push_back(create_random_individual());
    }
    
    Individual best_individual = population[0];
    for(size_t i = 1; i < population.size(); i++) {
        if(population[i].fitness > best_individual.fitness) {
            best_individual = population[i];
        }
    }
    
    int no_improvement = 0;
    
    for(int generation = 0; generation < max_generations; generation++) {
        std::vector<Individual> new_population;
        
        new_population.push_back(best_individual);
        
        while((int)new_population.size() < population_size) {
            Individual parent1 = tournament_selection(population);
            Individual parent2 = tournament_selection(population);
            
            Individual child = crossover(parent1, parent2);
            mutate(child);
            
            new_population.push_back(child);
        }
        
        population = new_population;
        
        Individual current_best = population[0];
        for(size_t i = 1; i < population.size(); i++) {
            if(population[i].fitness > current_best.fitness) {
                current_best = population[i];
            }
        }
        
        if(current_best.fitness > best_individual.fitness) {
            best_individual = current_best;
            no_improvement = 0;
        } else {
            if(generation > 0) {
                no_improvement++;
                if(no_improvement > max_generations / 10) {
                    generation = max_generations;
                }
            }
        }
    }
    
    return best_individual.genes;
}

std::vector<int> Genetic::get_neigbour_solution(const std::vector<int>& current) const {
    return get_zero_solution();
}

#ifndef GENETIC_H
#define GENETIC_H

#include "Knapsack.h"
#include <vector>
#include <random>

class Genetic: public Knapsack {
private:
    int population_size;
    double mutation_rate;
    double crossover_rate;
    int max_generations;
    
    struct Individual {
        std::vector<int> genes;
        int fitness;
        int weight;
        
        Individual(int n): genes(n, 0), fitness(0), weight(0) {}
        Individual(const std::vector<int>& g, int f, int w): genes(g), fitness(f), weight(w) {}
    };
    
    void calculate_fitness(Individual& ind) const;
    void repair_solution(std::vector<int>& solution) const;
    Individual create_random_individual() const;
    Individual crossover(const Individual& parent1, const Individual& parent2) const;
    void mutate(Individual& ind) const;
    Individual tournament_selection(const std::vector<Individual>& population) const;
    
public:
    Genetic(int n, int w, const std::vector<Item>& its, int pop_size = 100, double mut_rate = 0.01, double cross_rate = 0.8, int max_gen = 1000);
    
    virtual std::vector<int> get_zero_solution() const override;
    virtual std::vector<int> get_neigbour_solution(const std::vector<int>& current) const override;
};

#endif

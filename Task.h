#ifndef TASK_H
#define TASK_H

#include <vector>

class Task {
public:
    virtual ~Task() {}
    virtual std::vector<int> get_zero_solution() const = 0;
    virtual std::vector<int> get_neigbour_solution(const std::vector<int>& current) const = 0;
    virtual int how_good_is_solution(const std::vector<int>& sol) const = 0;
    virtual bool is_correct_solution(const std::vector<int>& sol) const = 0;
};

#endif

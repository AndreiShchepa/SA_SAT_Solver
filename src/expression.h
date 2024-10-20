#pragma once
#include <vector>

typedef std::vector<std::pair<std::size_t, bool>> ClauseType;
typedef std::vector<int> WeightType;
typedef std::vector<bool> Solution;

class Expression {
private:
    int num_vars = 0;
    int num_clauses = 0;
    double max_weight = 0.0;
    double avg_weight = 0.0;
    std::vector<ClauseType> clauses;
    WeightType weights;

public:
    Expression() = default;
    ~Expression() = default;

    int get_num_vars() const;
    int get_num_clauses() const;
    double get_max_weight() const;
    double get_avg_weight() const;
    WeightType get_weights();
    std::vector<ClauseType> get_clauses();
    void set_num_vars(int);
    void set_num_clauses(int);
    void add_clause(ClauseType);
    void set_weights(WeightType);
    void set_max_weight(double);
    void set_avg_weight(double);
};
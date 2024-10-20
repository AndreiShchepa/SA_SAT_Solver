#pragma once
#include <vector>
#include "expression.h"
#include "random_generator.h"

typedef struct state {
    int no_improve_iters;
    double curr_temp;
    int max_satisfied_clauses;
    int total_iters;
    double curr_weight;
    double best_weight;
    Solution curr_solution;
    Solution best_solution;
} state_t;

class Solver {
private:
    state_t in_state;
    RandomGenerator customRnd;
    std::vector<bool> currConfig;
    Expression expr;
    double final_temp;
    double factor_temp;
    double cooling_coef;
    int equilibrium_val;

    double calc_total_weight(const Solution&);
    double calc_init_temp(const Solution&);
    int gain_of_flip(Solution&, int);
    Solution generate_neighbour(Solution);
    Solution random_config();
    bool is_equilibrium(int) const;
    bool is_frozen() const;
    void cool_temp();
    bool is_clause_satisfied(ClauseType&, const Solution&);
    int count_satisfied_clauses(const Solution &config);
    void update_curr_solution(const Solution &config);
    void update_best_solution(const Solution &config);
    void print_stats();
    double calc_pure_best_weight();

public:
    Solver(const Expression& parsedExpr, double factor_temp, double cool_coef, int equilibrium) {
        this->expr = parsedExpr;
        this->in_state.curr_temp = 1.0;
        this->equilibrium_val = equilibrium;
        this->cooling_coef = cool_coef;
        this->factor_temp = factor_temp;
        this->in_state.total_iters = 0;
        this->in_state.no_improve_iters = 0;
        this->in_state.curr_solution = this->random_config();
        this->in_state.best_solution = this->in_state.curr_solution;
        this->in_state.max_satisfied_clauses = this->count_satisfied_clauses(this->in_state.curr_solution);
        this->in_state.curr_weight = this->calc_total_weight(this->in_state.curr_solution);
        this->in_state.best_weight = this->in_state.curr_weight;
    }
    ~Solver() = default;

    int solve();
};


#include "expression.h"
#include <utility>

int Expression::get_num_vars() const {
    return this->num_vars;
}

int Expression::get_num_clauses() const {
    return this->num_clauses;
}

WeightType Expression::get_weights() {
    return this->weights;
}

double Expression::get_max_weight() const {
    return this->max_weight;
}

double Expression::get_avg_weight() const {
    return this->avg_weight;
}

std::vector<ClauseType> Expression::get_clauses() {
    return this->clauses;
}

void Expression::set_num_vars(int num_vars) {
    this->num_vars = num_vars;
}

void Expression::set_num_clauses(int num_clauses) {
    this->num_clauses = num_clauses;
}

void Expression::add_clause(ClauseType clause) {
    this->clauses.push_back(clause);
}

void Expression::set_weights(WeightType weights) {
    this->weights = std::move(weights);
}

void Expression::set_max_weight(double max_weight) {
    this->max_weight = max_weight;
}

void Expression::set_avg_weight(double avg_weight) {
    this->avg_weight = avg_weight;
}
#include <iostream>
#include <bitset>
#include <utility>
#include <algorithm>
#include <fstream>
#include "solver.h"
#include "random_generator.h"

void Solver::print_stats() {
    for (size_t i = 0; i < this->in_state.best_solution.size(); ++i) {
        std::cout << (this->in_state.best_solution[i] ? 1 : -1) * static_cast<int>(i + 1)
                  << ((i + 1 < this->in_state.best_solution.size()) ? " " : "\n");
    }

    std::cerr << this->in_state.total_iters << " "
              << this->count_satisfied_clauses(this->in_state.best_solution) << " "
              << this->expr.get_num_clauses() << " "
              << this->calc_pure_best_weight() << std::endl;
}

bool Solver::is_frozen() const {
    return this->in_state.curr_temp < this->final_temp;
}

bool Solver::is_equilibrium(int step) const {
    return step > this->equilibrium_val;
}

void Solver::cool_temp() {
    this->in_state.curr_temp *= this->cooling_coef;
}

void Solver::update_curr_solution(const Solution& config) {
    this->in_state.curr_solution = config;
    this->in_state.max_satisfied_clauses = this->count_satisfied_clauses(config);
    this->in_state.curr_weight = this->calc_total_weight(config);
}

void Solver::update_best_solution(const Solution& config) {
    this->in_state.best_solution = config;
    this->in_state.best_weight = this->calc_total_weight(config);
}

bool Solver::is_clause_satisfied(ClauseType& clause, const Solution& config) {
    return std::any_of(clause.begin(), clause.end(), [&config](const auto& var) {
        return config[var.first - 1] == var.second;
    });
}

int Solver::count_satisfied_clauses(const Solution& config) {
    int count = 0;
    for (auto& clause : this->expr.get_clauses()) {
        if (is_clause_satisfied(clause, config)) {
            count++;
        }
    }

    return count;
}

Solution Solver::random_config() {
    Solution new_conf;
    new_conf.reserve(this->expr.get_num_vars());
    std::generate_n(std::back_inserter(new_conf), this->expr.get_num_vars(), [this]() {
        return this->customRnd.get_random_bool();
    });

    return new_conf;
}

double Solver::calc_init_temp(const Solution& init_config) {
    std::vector<double> deltaE;
    deltaE.reserve(1000);

    for (int i = 0; i < 1000; ++i) {
        Solution neighbor = generate_neighbour(init_config);
        double costDifference = calc_total_weight(neighbor) - calc_total_weight(init_config);

        if (costDifference < 0) {
            deltaE.push_back(std::abs(costDifference));
        }
    }

    double avgDeltaE = deltaE.empty() ? 0.0 :
                       std::accumulate(deltaE.begin(), deltaE.end(), 0.0) / deltaE.size();
    return -avgDeltaE / std::log(0.5);
}

double Solver::calc_total_weight(const Solution& config) {
    double sum = 0.0;
    const WeightType& weights = this->expr.get_weights();

    for (size_t i = 0; i < config.size(); ++i) {
        sum += config[i] * weights[i];
    }

    int num_satisfied_clauses = this->count_satisfied_clauses(config);
    int diff = this->expr.get_num_clauses() - num_satisfied_clauses;

    if (diff == 0) {
        return sum;
    }

    return sum / (this->expr.get_num_clauses() / 8.0) - this->expr.get_avg_weight() * diff * diff;
}

double Solver::calc_pure_best_weight() {
    double weight = 0.0;
    WeightType weights = this->expr.get_weights();
    for (size_t i = 0; i < this->in_state.best_solution.size(); i++) {
        weight += this->in_state.best_solution[i] * weights[i];
    }

    return weight;
}

int Solver::gain_of_flip(Solution& config, int var_id) {
    config[var_id] = !config[var_id];
    int satisfied_after = count_satisfied_clauses(config);
    config[var_id] = !config[var_id];

    return satisfied_after - this->in_state.max_satisfied_clauses;
}

Solution Solver::generate_neighbour(Solution curr_conf) {
    Solution new_conf = std::move(curr_conf);
    int prob_of_flip = this->customRnd.get_random_int(0, 99);

    if (prob_of_flip % 10 == 0) {
        return this->random_config();
    }

    if (this->count_satisfied_clauses(new_conf) != this->expr.get_num_clauses()) {
        for (ClauseType &clause : this->expr.get_clauses()) {
            if (this->is_clause_satisfied(clause, new_conf)) {
                continue;
            }

            for (std::pair<size_t, bool> par : clause) {
                int gain = gain_of_flip(new_conf, par.first - 1);
                if (gain > 0) {
                    new_conf[par.first - 1] = !new_conf[par.first - 1];
                    return new_conf;
                }
            }
        }
    }

    int random_num_vars = this->customRnd.get_random_int(1, this->expr.get_num_vars()/2);
    for (int i = 0; i < random_num_vars; i++) {
        int random_idx = this->customRnd.get_random_int(0, this->expr.get_num_vars() - 1);
        new_conf[random_idx] = !new_conf[random_idx];
    }

    return new_conf;
}

int Solver::solve() {
    this->in_state.curr_temp = this->calc_init_temp(this->in_state.curr_solution);
    this->final_temp = this->in_state.curr_temp / this->factor_temp;

//    std::ofstream outfile_clauses("output_clauses.txt");
//    std::ofstream outfile_weight("output_weight.txt");

    while(!is_frozen()) {
        int i = 0;
        while (!is_equilibrium(i++)) {
            Solution rnd_solution = generate_neighbour(this->in_state.curr_solution);
            double rnd_solution_weight = this->calc_total_weight(rnd_solution);

            if (rnd_solution_weight > this->in_state.curr_weight) {
                if (rnd_solution_weight > this->in_state.best_weight) {
                    this->update_best_solution(rnd_solution);
                }

                this->in_state.no_improve_iters = 0;
                this->update_curr_solution(rnd_solution);
            }
            else {
                double diff = this->in_state.curr_weight - rnd_solution_weight;
                double acceptance_probability = exp(-diff / this->in_state.curr_temp);
                double rnd_num = this->customRnd.get_random_double(0.0, 1.0);

                if (rnd_num < acceptance_probability) {
                    this->in_state.no_improve_iters = 0;
                    this->update_curr_solution(rnd_solution);
                } else {
                    this->in_state.no_improve_iters++;
                }
            }

//             outfile_weight << this->in_state.total_iters << " "
//                            << this->calc_total_weight(this->in_state.curr_solution) << " "
//                            << this->calc_total_weight(this->in_state.best_solution) << "\n";
//
//             outfile_clauses << this->in_state.total_iters << " "
//                             << this->count_satisfied_clauses(this->in_state.curr_solution) << "\n";

            if (this->in_state.no_improve_iters > 1000) {
                goto end_program;
            }

            this->in_state.total_iters++;
        }

        cool_temp();
    }

end_program:
//    outfile_clauses.close();
//    outfile_weight.close();
    this->print_stats();
    return 0;
}

#include <iostream>
#include <sstream>
#include <unistd.h>
#include "problem_loader.h"

int ProblemLoader::set_istream(const char *filename, bool file_flag) {
    if (file_flag) {
        this->input_file.open(filename);
        if (!this->input_file.is_open()) {
            std::cerr << "Error: Unable to open the input file: " << filename << std::endl;
            return 1;
        }
        this->input_stream = &(this->input_file);
    } else {
        this->input_stream = &std::cin;
    }

    return 0;
}

void ProblemLoader::arg_parser(int argc, char **argv) {
    int option, ret_val;

    while ((option = getopt(argc, argv, "T:e:a:f:")) != -1) {
        switch (option) {
            case 'T':
                this->args.factor_temp = std::atof(optarg);
                break;
            case 'e':
                this->args.equilibrium = std::atoi(optarg);
                break;
            case 'a':
                this->args.cool_coef = std::atof(optarg);
                break;
            case 'f':
                ret_val = this->set_istream(optarg, true);
                if (ret_val) {
                    std::cerr << "Wrong input filename: " << optarg << std::endl;
                    exit(EXIT_FAILURE);
                }

                break;
            case '?':
                std::cerr << "Usage: " << argv[0] << " -T factor_temp -e equilibrium -a cool_coef -f input_file" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    if (!this->input_stream) {
        this->set_istream(optarg, false);
    }
}

args_t ProblemLoader::get_args() {
    return this->args;
}

int ProblemLoader::parse_expr(Expression *expr) {
    if (!expr) return 1;

    int num_vars, num_clauses;
    std::string line;

    while (std::getline(*(this->input_stream), line)) {
        if (line.empty() || line[0] == 'c') continue; // Skip comments

        if (line[0] == 'p') {
            std::istringstream iss(line);
            std::string header, format;

            if (!(iss >> header >> format >> num_vars >> num_clauses) || format != "mwcnf") {
                std::cerr << "Error: Invalid problem header." << std::endl;
                return 1;
            }
            expr->set_num_vars(num_vars);
            expr->set_num_clauses(num_clauses);
        } else if (line[0] == 'w') {
            std::istringstream iss(line.substr(2));
            parse_weights(iss, expr, num_vars);
        } else {
            std::istringstream iss(line); // Process the entire line for clauses
            if (!parse_clause(iss, expr)) return 1;
        }
    }
    return 0;
}

void ProblemLoader::parse_weights(std::istringstream &iss, Expression *expr, int num_vars) {
    double weight, max_weight = 0.0;
    WeightType weights;
    weights.reserve(num_vars);

    while (iss >> weight && weight != 0) {
        weights.push_back(weight);
        max_weight += weight;
    }

    if (weights.size() != static_cast<size_t>(num_vars)) {
        std::cerr << "Error: Number of weights does not match number of variables." << std::endl;
        return;
    }

    expr->set_weights(weights);
    expr->set_avg_weight(max_weight / weights.size());
    expr->set_max_weight(max_weight);
}

bool ProblemLoader::parse_clause(std::istringstream &iss, Expression *expr) {
    ClauseType clause;
    int literal;

    while (iss >> literal) {
        if (literal == 0) break; // End of clause
        clause.emplace_back(std::abs(literal), literal > 0);
    }

    if (clause.empty()) {
        std::cerr << "Error: Empty clause encountered." << std::endl;
        return false;
    }

    expr->add_clause(clause);
    return true;
}
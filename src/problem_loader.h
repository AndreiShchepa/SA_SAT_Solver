#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include "expression.h"

typedef struct Arguments {
    double factor_temp = 100;
    int equilibrium = 300;
    double cool_coef = 0.975;
} args_t;

class ProblemLoader {
private:
    std::ifstream input_file;
    std::istream* input_stream = nullptr;
    args_t args;

    int set_istream(const char*, bool);
    void parse_weights(std::istringstream &, Expression *, int);
    bool parse_clause(std::istringstream &, Expression *);

public:
    ProblemLoader() = default;
    ~ProblemLoader() = default;

    void arg_parser(int, char**);
    int parse_expr(Expression*);
    args_t get_args();
};
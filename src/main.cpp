#include "problem_loader.h"
#include "expression.h"
#include "solver.h"

int main(int argc, char **argv) {
    ProblemLoader loader;
    loader.arg_parser(argc, argv);

    Expression expr;
    if (loader.parse_expr(&expr))
        return 1;

    args_t args = loader.get_args();
    Solver solver = Solver(expr, args.factor_temp, args.cool_coef, args.equilibrium);
    solver.solve();

    return 0;
}
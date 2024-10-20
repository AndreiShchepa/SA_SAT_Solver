import subprocess
import os
import argparse
from collections import defaultdict

from config import BLACKBOX_OUTPUT_DIR


class MAXWSATSolver:
    def __init__(self, data, program_path):
        self.data = data
        self.program_path = program_path
        self.stats = defaultdict(lambda: defaultdict(float))
        self.clauses_stats = defaultdict(int)
        self.error_stats = defaultdict(int)

    def run_algorithm(self, problem_file):
        for _ in range(10):
            command = [self.program_path, "-f", problem_file, "-e", "300", "-T", "50", "-a", "0.975"]
            process = subprocess.run(command, capture_output=True, text=True)
            stdout, stderr = process.stdout.strip(), process.stderr.strip()
            self.collect_run_statistics(stderr, problem_file)

    def collect_run_statistics(self, stderr, problem_file):
        iters, solved, expected, weight = map(int, stderr.split())
        filename_key = self.extract_filename_key(problem_file)

        if filename_key not in self.data:
            print(f"Warning: Data not found for {filename_key}. Skipping.")
            return

        self.clauses_stats[solved] += 1
        if solved == expected:
            relative_error = abs(self.data[filename_key]['opt_weight'] - weight) / self.data[filename_key]['opt_weight']
            self.error_stats[relative_error] += 1

            self.stats[filename_key]['total_runs'] += 1
            self.stats[filename_key]['total_iterations'] += iters
            self.stats[filename_key]['solved'] += 1
            self.stats[filename_key]['relative_error_sum'] += relative_error
            if weight == self.data[filename_key]['opt_weight']:
                self.stats[filename_key]['optimal_solved'] += 1

    @staticmethod
    def extract_filename_key(filename):
        return os.path.basename(filename).replace(".mwcnf", "").lstrip('w')

    def output_stats(self, output_file_prefix):
        os.makedirs(os.path.dirname(output_file_prefix), exist_ok=True)

        total_runs = sum(stat['total_runs'] for stat in self.stats.values())
        total_solved = sum(stat['solved'] for stat in self.stats.values())
        total_optimal_solved = sum(stat['optimal_solved'] for stat in self.stats.values())
        total_relative_error_sum = sum(stat['relative_error_sum'] for stat in self.stats.values())
        total_iterations = sum(stat['total_iterations'] for stat in self.stats.values())

        avg_relative_error = total_relative_error_sum / total_solved if total_solved > 0 else 0
        avg_iterations = total_iterations / total_runs if total_runs > 0 else 0
        success_rate = (total_solved / total_runs) * 100 if total_runs > 0 else 0
        optimal_success_rate = (total_optimal_solved / total_runs) * 100 if total_runs > 0 else 0

        with open(f"{output_file_prefix}_aggregated_stats.out", 'w') as file:
            file.write(f"Total runs: {total_runs}\n")
            file.write(f"Average iterations per run: {avg_iterations:.2f}\n")
            file.write(f"Success rate: {success_rate:.2f}%\n")
            file.write(f"Optimal solution success rate: {optimal_success_rate:.2f}%\n")
            file.write(f"Average relative error: {avg_relative_error:.4f}\n")

        with open(f"{output_file_prefix}_clauses_stats.out", 'w') as file:
            for num_clauses, count in self.clauses_stats.items():
                file.write(f"{num_clauses}: {count}\n")

        with open(f"{output_file_prefix}_relative_error_stats.out", 'w') as file:
            for error, count in sorted(self.error_stats.items()):
                file.write(f"{error:.4f}: {count}\n")


def parse_answers_file(file_path):
    answers = {}
    with open(file_path, 'r') as file:
        for line in file:
            filename, opt_weight, *solution = line.split()
            answers[filename] = {"opt_weight": int(opt_weight), "solution": ' '.join(solution).strip()}
    return answers


def get_data(directory_path, answers_file):
    answers = parse_answers_file(answers_file)
    data = {}
    for filename in os.listdir(directory_path):
        if filename.endswith(".mwcnf"):
            filename_key = filename.replace(".mwcnf", "").lstrip('w')
            if filename_key in answers:
                data[filename_key] = answers[filename_key]
            else:
                print(f"Warning: No answer data found for {filename_key}")
    return data


def main():
    parser = argparse.ArgumentParser(description='Run MAXWSAT solver for blackbox tests.')
    parser.add_argument('--program-path', required=True, help='Path to the SA program')
    parser.add_argument('--dir-path', required=True, help='Path to the directory with problems')

    args = parser.parse_args()
    problems_dir = os.path.join(args.dir_path, "wuf-data")
    answers_file = os.path.join(args.dir_path, "wuf-opt.dat")
    output_file_prefix = os.path.join(BLACKBOX_OUTPUT_DIR, f"{os.path.basename(args.dir_path)}")

    data = get_data(problems_dir, answers_file)
    solver = MAXWSATSolver(data, args.program_path)

    for problem_file in os.listdir(problems_dir):
        if problem_file.endswith(".mwcnf"):
            solver.run_algorithm(os.path.join(problems_dir, problem_file))

    solver.output_stats(output_file_prefix)


if __name__ == '__main__':
    main()

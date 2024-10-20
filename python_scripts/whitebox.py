import subprocess
import os
from config import WHITEBOX_DATA_FACTOR_ANALYSE_DIR, PROBLEMS_DIR_NAME, WHITEBOX_TABLE
import argparse
from collections import defaultdict


def ensure_directory_exists(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)


class MAXWSATSolver:
    def __init__(self, data, program_path, cool_coef, factor_temp, equilibrium):
        self.data = data
        self.program_path = program_path
        self.cool_coef = cool_coef
        self.factor_temp = factor_temp
        self.equilibrium = equilibrium
        self.stats = defaultdict(lambda: defaultdict(lambda: defaultdict(float)))

    def run_algorithm(self, subdir, letter):
        answers = self.data[subdir][letter]['answers']
        for val in self.data[subdir][letter]['data']:
            for _ in range(5):
                command = f"{self.program_path} -e {self.equilibrium} -T {self.factor_temp} -a {self.cool_coef} -f {val}"
                process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                stdout, stderr = process.communicate()
                solution, stderr_decoded = stdout.decode("utf-8").rstrip(), stderr.decode("utf-8").rstrip()
                self.compare_results(stderr_decoded, answers, self.stats[subdir][letter], val)

    def compare_results(self, stderr, answers, stats, filename):
        vals = stderr.split()
        iters, solved, expected, weight = int(vals[0]), int(vals[1]), int(vals[2]), int(vals[3])
        filename_key = self.extract_filename_key(filename)

        stats['total_average_iterations'] += iters
        stats['num_runs'] += 1
        if solved == expected:
            stats["success_percentage"] += 1
            if weight == answers[filename_key]['opt_weight']:
                stats["success_optimal_percentage"] += 1
            else:
                stats["average_relative_error"] += abs(answers[filename_key]['opt_weight'] - weight) / \
                                                   answers[filename_key]['opt_weight']

    @staticmethod
    def extract_filename_key(filename):
        return filename.split("/")[-1].replace(".mwcnf", "").replace("w", "")

    def output_stats(self):
        ensure_directory_exists(WHITEBOX_TABLE)
        file_name = f"{WHITEBOX_TABLE}/{self.cool_coef}_{self.factor_temp}_{self.equilibrium}.stats.out"
        with open(file_name, 'w') as file:
            for subdir, letters in self.stats.items():
                file.write(f"dataset: {subdir}\n")
                for letter, stats in letters.items():
                    file.write(f"Letter: {letter}\n")
                    num_runs = stats['num_runs']
                    file.write(f"Number of runs: {num_runs}\n")
                    for stat, value in stats.items():
                        if stat != 'num_runs':
                            average_value = value / num_runs
                            file.write(f"{stat}: {average_value:.4f}\n")
                    file.write("\n")


def parse_answers_file_to_dict(file_path):
    answers = {}
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.split()
            filename = parts[0]
            opt_weight = int(parts[1])
            solution = ' '.join(parts[2:-1])
            answers[filename] = {"opt_weight": opt_weight, "solution": solution.rstrip()}
    return answers


def get_data(directory_path):
    ensure_directory_exists(directory_path)
    data = dict()

    for subdir in os.listdir(directory_path):
        subdir_path = os.path.join(directory_path, subdir)
        ensure_directory_exists(subdir_path)
        data[subdir] = {"M": {"data": [], "answers": []},
                        "N": {"data": [], "answers": []},
                        "Q": {"data": [], "answers": []},
                        "R": {"data": [], "answers": []}}

        for itm in os.listdir(subdir_path):
            parts = itm.split('-')
            if len(parts) == 3 and parts[2].startswith('data'):
                letter = parts[1]
                for file in os.listdir(os.path.join(subdir_path, itm)):
                    data[subdir][letter]['data'].append(os.path.join(subdir_path, f'{PROBLEMS_DIR_NAME.format(letter)}/{file}'))
            elif len(parts) == 3 and parts[2].startswith('opt'):
                letter = parts[1]
                data[subdir][letter]['answers'] = parse_answers_file_to_dict(os.path.join(subdir_path, itm))

    return data


def main():
    parser = argparse.ArgumentParser(description='Run MAXWSAT solver for whitebox tests.')
    parser.add_argument('--program-path', required=True, help='Path to the SA program')
    parser.add_argument('--cool-coef', type=float, required=True, help='Cooling coefficient')
    parser.add_argument('--factor-temp', type=float, required=True, help='Factor temperature')
    parser.add_argument('--equilibrium', type=int, required=True, help='Equilibrium length')

    args = parser.parse_args()

    data_dict = get_data(WHITEBOX_DATA_FACTOR_ANALYSE_DIR)

    solver = MAXWSATSolver(data_dict, args.program_path, args.cool_coef, args.factor_temp, args.equilibrium)

    for subdir in data_dict.keys():
        for letter in data_dict[subdir].keys():
            solver.run_algorithm(subdir, letter)

    solver.output_stats()


if __name__ == '__main__':
    main()

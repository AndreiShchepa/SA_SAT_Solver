import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
import os
import sys


def read_data_from_file(file_path, num_columns):
    data = []
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.split()
            if len(parts) == num_columns:
                data.append(tuple(map(float, parts[:num_columns])))
    return zip(*data)


def plot_data(y_label, x_data, y_data, y2_data=None, optimum_weight=None, file_name='plot.pdf', title='Plot'):
    plt.figure(figsize=(10, 6))

    if y2_data:
        plt.plot(x_data, y_data, label='Weight', color='#0080FF')
        plt.plot(x_data, y2_data, label='Best weight', color='coral')
        if optimum_weight is not None:
            plt.axhline(y=optimum_weight, color='green', linestyle='--', label=f'Optimum Weight: {optimum_weight}')
        plt.legend()
    else:
        plt.plot(x_data, y_data, color='#0080FF')

    plt.xlabel('Iterations', fontsize=12)
    plt.ylabel(y_label, fontsize=12)
    plt.title(title, fontsize=14)

    plt.gca().yaxis.set_major_locator(MaxNLocator(integer=True))

    plt.savefig(file_name)
    plt.show()
    plt.close()


def main(directory_path, optimum_weight):
    clauses_file_path = os.path.join(directory_path, 'output_clauses.txt')
    weight_file_path = os.path.join(directory_path, 'output_weight.txt')

    x_data, y_data = read_data_from_file(clauses_file_path, 2)
    plot_data("Satisfied clauses", x_data, y_data, file_name=os.path.join(directory_path, 'clauses_plot.pdf'), title='Satisfied Clauses Over Iterations')

    x_data, y_data, y2_data = read_data_from_file(weight_file_path, 3)
    plot_data("Weight", x_data, y_data, y2_data, optimum_weight, file_name=os.path.join(directory_path, 'weight_plot.pdf'), title='Weight Over Iterations')


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <path_to_directory_with_data_files> <optimum_weight>")
        sys.exit(1)

    directory = sys.argv[1]
    opt_weight = int(sys.argv[2])

    if not os.path.isdir(directory):
        print(f"The provided directory path {directory} does not exist.")
        sys.exit(1)

    main(directory, opt_weight)

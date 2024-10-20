import sys
import pandas as pd
import matplotlib.pyplot as plt

def calculate_cdf(file):
    df = pd.read_csv(file, sep=':', names=['Value', 'Count'])

    df.sort_values('Value', inplace=True)
    df['CumulativeCount'] = df['Count'].cumsum()
    df['CDF'] = df['CumulativeCount'] / df['CumulativeCount'].iloc[-1]
    return df


def plot_cdfs(dataframes, legends):
    plt.figure(figsize=(10, 6))
    #colors = [(0, 0.6, 0.3), (1, 0.4, 0.4)]
    colors = [(0.2, 0.6, 1), (1, 0.6, 0.2)]

    for i, df in enumerate(dataframes):
        plt.step(df['Value'], df['CDF'], where='post', label=legends[i], color=colors[i % len(colors)])

    plt.xlabel('Relative error')
    plt.ylabel('Probability')
    plt.legend()
    plt.grid(True)
    plt.savefig("file.pdf", bbox_inches='tight', format='pdf')


if len(sys.argv) < 5:
    print("Please provide at least two filenames and two legend names as arguments.")
    sys.exit(1)

dataframes = []
legends = [sys.argv[3], sys.argv[4]]
for filename in sys.argv[1:3]:
    cdf_df = calculate_cdf(filename)
    dataframes.append(cdf_df)

plot_cdfs(dataframes, legends)
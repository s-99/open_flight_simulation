import matplotlib.pyplot as plt
import pandas as pd


def plot_data(ax, df, y_name, y_label=None, show_xlabel=False):
    ax.plot(df['t'], df[y_name], linestyle='-', color='b')
    ax.set_ylabel(y_label if y_label else y_name)
    if show_xlabel:
        ax.set_xlabel('t')

    ax.grid(True)


def plot_compare(ax, data, y_label, show_xlabel=False):
    """

    :param ax:
    :param data: [(df, y_name, label), (df, y_name, label)]
    :param y_label:
    :param show_xlabel:
    :return:
    """
    for df, y_name, label in data:
        ax.plot(df['t'], df[y_name], linestyle='-', label=label)
    ax.set_ylabel(y_label)
    if show_xlabel:
        ax.set_xlabel('t')
    ax.legend()
    ax.grid(True)
import matplotlib.pyplot as plt
import pandas as pd
from plot_util import plot_compare, plot_data


def plot_lon():
    aero = pd.read_csv('../bin/aero_rk4.csv')
    dyn1 = pd.read_csv('../bin/dyn_rk1.csv')
    dyn4 = pd.read_csv('../bin/dyn_rk4.csv')

    fig, ax = plt.subplots(4, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], aero, 'i/de', r"$\delta_e$")
    plot_data(ax[1][0], aero, 'M', "$M$")
    plot_compare(ax[2][0], [(dyn1, 'alpha', 'rk1'), (dyn4, 'alpha', 'rk4')], r"$\alpha (deg)$")
    plot_compare(ax[3][0], [(dyn1, 'q', 'rk1'), (dyn4, 'q', 'rk4')], r"$q (rad/s)$", True)
    plot_compare(ax[0][1], [(dyn1, 'theta', 'rk1'), (dyn4, 'theta', 'rk4')], r"$\theta (deg)$")
    plot_compare(ax[1][1], [(dyn1, 'vel', 'rk1'), (dyn4, 'vel', 'rk4')], r"$V (m/s)$")
    plot_compare(ax[2][1], [(dyn1, 'h', 'rk1'), (dyn4, 'h', 'rk4')], r"$H$")
    plot_compare(ax[3][1], [(dyn1, 'alpha_d', 'rk1'), (dyn4, 'alpha_d', 'rk4')], r"$\dot{\alpha} (rad/s)$", True)

    fig.tight_layout()
    plt.show()


def plot_lon1():
    aero = pd.read_csv('../bin/aero.csv')
    dyn = pd.read_csv('../bin/dyn.csv')
    dyn_aero = pd.read_csv('../bin/dyn_aero.csv')

    fig, ax = plt.subplots(4, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], aero, 'i/de', r"$\delta_e$")
    plot_data(ax[1][0], aero, 'M', "$M$")
    plot_compare(ax[2][0], [(dyn, 'alpha', 'split'), (dyn_aero, 'alpha', 'combine')], r"$\alpha (deg)$")
    plot_compare(ax[3][0], [(dyn, 'q', 'split'), (dyn_aero, 'q', 'combine')], r"$q (rad/s)$", True)
    plot_compare(ax[0][1], [(dyn, 'theta', 'split'), (dyn_aero, 'theta', 'combine')], r"$\theta (deg)$")
    plot_compare(ax[1][1], [(dyn, 'vel', 'split'), (dyn_aero, 'vel', 'combine')], r"$V (m/s)$")
    plot_compare(ax[2][1], [(dyn, 'h', 'split'), (dyn_aero, 'h', 'combine')], r"$H$")
    plot_compare(ax[3][1], [(dyn, 'alpha_d', 'split'), (dyn_aero, 'alpha_d', 'combine')], r"$\dot{\alpha} (rad/s)$", True)

    fig.tight_layout()
    plt.show()


def plot_lon2():
    rk4 = pd.read_csv('../bin/dyn_aero.csv')
    ab4 = pd.read_csv('../bin/dyn_aero_adams.csv')

    fig, ax = plt.subplots(4, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], rk4, 'i/de', r"$\delta_e$")
    plot_data(ax[1][0], rk4, 'v/M', "$M$")
    plot_compare(ax[2][0], [(rk4, 'alpha', 'rk4'), (ab4, 'alpha', 'ab4')], r"$\alpha (deg)$")
    plot_compare(ax[3][0], [(rk4, 'q', 'rk4'), (ab4, 'q', 'ab4')], r"$q (rad/s)$", True)
    plot_compare(ax[0][1], [(rk4, 'theta', 'rk4'), (ab4, 'theta', 'ab4')], r"$\theta (deg)$")
    plot_compare(ax[1][1], [(rk4, 'vel', 'rk4'), (ab4, 'vel', 'ab4')], r"$V (m/s)$")
    plot_compare(ax[2][1], [(rk4, 'h', 'rk4'), (ab4, 'h', 'ab4')], r"$H$")
    plot_compare(ax[3][1], [(rk4, 'alpha_d', 'rk4'), (ab4, 'alpha_d', 'ab4')], r"$\dot{\alpha} (rad/s)$", True)

    fig.tight_layout()
    plt.show()


if __name__ == "__main__":
    plot_lon2()

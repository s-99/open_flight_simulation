import matplotlib.pyplot as plt
import pandas as pd
from plot_util import plot_data, plot_compare, plot_compare1


def plot_lon():
    dyn = pd.read_csv('../bin/dyn.csv')
    aero = pd.read_csv('../bin/aero.csv')

    fig, ax = plt.subplots(4, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], aero, 'i/de', r"$\delta_e$")
    plot_data(ax[1][0], aero, 'M', "$M$")
    plot_data(ax[2][0], dyn, 'alpha', r"$\alpha (deg)$")
    plot_data(ax[3][0], dyn, 'q', r"$q (rad/s)$", True)
    plot_data(ax[0][1], dyn, 'theta', r"$\theta (deg)$")
    plot_data(ax[1][1], dyn, 'vel', r"$V (m/s)$")
    plot_data(ax[2][1], dyn, 'mach', r"$Mach$")
    plot_data(ax[3][1], dyn, 'alpha_d', r"$\dot{\alpha} (rad/s)$", True)

    fig.tight_layout()
    plt.show()


def plot_lat():
    dyn = pd.read_csv('../bin/dyn_lat.csv')
    aero = pd.read_csv('../bin/aero_lat.csv')

    fig, ax = plt.subplots(4, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], aero, 'i/da', r"$\delta_a$")
    plot_data(ax[1][0], aero, 'i/dr', r"$\delta_r$")
    plot_data(ax[2][0], dyn, 'p', "$p$")
    plot_data(ax[3][0], dyn, 'r', r"$r$", True)
    plot_data(ax[0][1], dyn, 'beta', r"$\beta (deg)$")
    plot_data(ax[1][1], dyn, 'phi', r"$\phi (deg)$")
    plot_data(ax[2][1], dyn, 'psi', r"$\psi (deg)$")
    plot_data(ax[3][1], dyn, 'h', r"$H$", True)

    fig.tight_layout()
    plt.show()


def plot_v():
    dyn = pd.read_csv('../bin/dyn_lat.csv')
    aero = pd.read_csv('../bin/aero_lat.csv')

    fig, ax = plt.subplots(3, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], dyn, 'u')
    plot_data(ax[1][0], dyn, 'v')
    plot_data(ax[2][0], dyn, 'w', show_xlabel=True)
    plot_data(ax[0][1], dyn, 'x')
    plot_data(ax[1][1], dyn, 'y')
    plot_data(ax[2][1], dyn, 'z', show_xlabel=True)

    fig.tight_layout()
    plt.show()


def plot():
    fcs = pd.read_csv('../bin/fcs.csv')
    dyn = pd.read_csv('../bin/dyn.csv')
    aero = pd.read_csv('../bin/aero.csv')

    fig, ax = plt.subplots(5, 2, figsize=(10, 6), sharex=True)

    plot_data(ax[0][0], fcs, 'block4', r"$s_{push}$")
    plot_data(ax[1][0], aero, 'i/de', r"$\delta_e$")
    plot_compare(ax[2][0], [(dyn, 'alpha', r"$\alpha (deg)$"), (dyn, 'theta', r"$\theta (deg)$")], "")
    plot_compare(ax[3][0], [(dyn, 'q', r"$q (rad/s)$"), (dyn, 'alpha_d', r"$\dot{\alpha} (rad/s)$")], "")
    plot_data(ax[4][0], dyn, 'vel', r"$V (m/s)$", True)

    plot_compare(ax[0][1], [(aero, 'i/da', r"$\delta_a$"), (aero, 'i/dr', r"$\delta_r$")], "")
    plot_compare(ax[1][1], [(dyn, 'p', "$p$"), (dyn, 'r', r"$r$")], "")
    plot_compare(ax[2][1], [(dyn, 'phi', r"$\phi (deg)$"), (dyn, 'psi', r"$\psi (deg)$")], "")
    plot_data(ax[3][1], dyn, 'beta', r"$\beta (deg)$", True)
    # tCm_basic,Cm_basic,tCmq,Cmq,tCmadot,Cmadot,tCmDe,CmDe,tCmDf1L,CmDf1L,tCmDf1R,CmDf1R,

    fig.tight_layout()
    plt.show()


if __name__ == "__main__":
    plot()

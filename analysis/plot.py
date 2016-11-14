#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import argparse

def loadVariables(filename, N):
    var = np.fromfile(filename, dtype=np.dtype(np.double))
    density = var[:N]
    pressure = var[N:2*N]
    velocity = var[2*N:]

    return pressure, density, velocity


def plotVariables(axes, pressure, density, velocity, N, numerical=False):
    x = np.linspace(0, 1, num=N)
    if numerical:
        linestyle = 'x'
    else:
        linestyle = ''

    axes[0].plot(x, pressure, linestyle)
    axes[0].set_ylabel("Pressure")
    axes[1].plot(x, density, linestyle)
    axes[1].set_ylabel("Density")
    axes[2].plot(x, velocity, linestyle)
    axes[2].set_ylabel("Velocity")


def main():
    parser = argparse.ArgumentParser(
        description='Plotter script for 1D fluids'
    )
    parser.add_argument('filenames', help='Input file name', nargs='+')
    parser.add_argument('--over', help='Data file to plot over')
    parser.add_argument('-N', help='Number of grid points',
                        type=int, required=True)
    parser.add_argument('-c', '--command', metavar='command',
                        choices=['show', 'save'],
                        help='Plotting command to run',
                        required=True)
    args = parser.parse_args()

    for filename in args.filenames:
        print("Plotting " + filename)
        f, axes = plt.subplots(1, 3, sharex=True)
        pressure, density, velocity = loadVariables(filename, args.N)
        plotVariables(axes, pressure, density, velocity, args.N)
        if(args.over):
            pressure, density, velocity = loadVariables(args.over, args.N)
            plotVariables(axes, pressure, density, velocity, args.N)

        if(args.command == "show"):
            plt.show()
        elif(args.command == "save"):
            outFilename = ''.join(filename.split('.')[:-1]) + ".png"
            plt.savefig(outFilename, format='png')

        plt.close()


if __name__ == '__main__':
    main()

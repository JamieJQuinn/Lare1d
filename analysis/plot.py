#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import numpy as np
import argparse

def loadVariables(filename, N):
    var = np.fromfile(filename, dtype=np.dtype(np.double))
    density = var[:N]
    pressure = var[N:2*N]
    velocity = var[2*N:]

    return pressure, density, velocity


def plotVariables(pressure, density, velocity, N):
    f, axes = plt.subplots(3, sharex=True)

    x = np.linspace(0, 1, num=N)
    axes[0].plot(x, pressure)
    axes[0].set_ylabel("Pressure")
    axes[1].plot(x, density)
    axes[1].set_ylabel("Density")
    axes[2].plot(x, velocity)
    axes[2].set_ylabel("Velocity")


def plot(command, filename, N):
    pressure, density, velocity = loadVariables(filename, N)
    plotVariables(pressure, density, velocity, N)
    if(command == "show"):
        plt.show()
    elif(command == "save"):
        outFilename = ''.join(filename.split('.')[:-1]) + ".png"
        plt.savefig(outFilename, format='png')

    plt.close()


def main():
    parser = argparse.ArgumentParser(
        description='Plotter script for 1D fluids'
    )
    parser.add_argument('filenames', help='Input file name', nargs='+')
    parser.add_argument('-N', help='Number of grid points',
                        type=int, required=True)
    parser.add_argument('-c', '--command', metavar='command',
                        choices=['show', 'save'],
                        help='Plotting command to run',
                        required=True)
    args = parser.parse_args()

    for filename in args.filenames:
        print("Plotting " + filename)
        plot(args.command, filename, args.N)


if __name__ == '__main__':
    main()

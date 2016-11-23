#!/usr/bin/env python3

import numpy as np
import argparse


def setICsRiemann(x, pressure, density, velocity):
    xRange = (x < 0.5)
    pressure[xRange] = 128.0
    density[xRange]  = 256.0/49.0
    velocity[xRange] = 0.0

    xRange = (x >= 0.5)
    pressure[xRange] = 1.0
    density[xRange]  = 2.0/25.0
    velocity[xRange] = 0.0


def setICsSod(x, pressure, density, velocity):
    xRange = (x < 0.5)
    pressure[xRange] = 1.0
    density[xRange]  = 1.0
    velocity[xRange] = 0.0

    xRange = (x >= 0.5)
    pressure[xRange] = 0.1
    density[xRange]  = 0.125 
    velocity[xRange] = 0.0


def main():
    parser = argparse.ArgumentParser(
        description='Initial condition creation tool'
    )
    parser.add_argument('problem', help='Specific problem',
                       choices=['sod', 'riemann'])
    parser.add_argument('-N', help='Number of grid points',
                        type=int, required=True)
    parser.add_argument('-o', help='Output file', required=True)
    args = parser.parse_args()

    x = np.linspace(0, 1, num=args.N)
    pressure = np.zeros_like(x)
    density  = np.zeros_like(x)
    velocity = np.zeros_like(x)

    if args.problem == 'riemann':
        setICsRiemann(x, pressure, density, velocity)
    elif args.problem == 'sod':
        setICsSod(x, pressure, density, velocity)

    with open(args.o, 'wb') as fp:
        pressure.tofile(fp)
        density.tofile(fp)
        velocity.tofile(fp)


if __name__ == '__main__':
    main()

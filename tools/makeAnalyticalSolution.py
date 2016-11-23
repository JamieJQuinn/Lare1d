#!/usr/bin/env python3

import numpy as np
import argparse


def setASRiemann(x, t, pressure, density, velocity):
    xRange = (x < 0.5-7*t)
    pressure[xRange] = 128.0
    density[xRange]  = 256.0/49.0
    velocity[xRange] = 0.0

    xRange = (x >= 0.5 - 7*t) and (x < 0.5 + 7.0/2*t)
    sigma = (x[xRange] - 0.5)/t # Characteristic
    pressure[xRange] = 128.0 *    (2.0/3 - 1.0/21*sigma)**4
    density[xRange]  = 256.0/49.0*(2.0/3 - 1.0/21*sigma)**2
    velocity[xRange] = 2.0/3*(sigma + 7.0)

    xRange = (x >= 0.5 + 7.0/2*t) and (x < 0.5 + 7*t)
    pressure[xRange] = 8.0
    density[xRange]  = 64.0/49.0
    velocity[xRange] = 7.0

    xRange = (x >= 0.5 + 7*t) and (x < 0.5 + 25.0/2*t)
    pressure[xRange] = 8.0
    density[xRange]  = 2.0/1.0
    velocity[xRange] = 7.0

    xRange = (x >= 0.5 + 25/2)
    pressure[xRange] = 1.0
    density[xRange]  = 2.0/25.0
    velocity[xRange] = 0.0


def main():
    parser = argparse.ArgumentParser(
        description='Analytical solution creation tool'
    )
    parser.add_argument('problem', help='Specific problem',
                       choices=['riemann'])
    parser.add_argument('-N', help='Number of grid points',
                        type=int, required=True)
    parser.add_argument('-o', help='Output file', required=True)
    parser.add_argument('-t', help='Time at which solution occurs', required=True)
    args = parser.parse_args()

    x = np.linspace(0, 1, num=args.N)
    pressure = np.zeros_like(x)
    density  = np.zeros_like(x)
    velocity = np.zeros_like(x)

    if args.problem == 'riemann':
        setASRiemann(x, pressure, density, velocity)

    with open(args.o, 'wb') as fp:
        pressure.tofile(fp)
        density.tofile(fp)
        velocity.tofile(fp)


if __name__ == '__main__':
    main()

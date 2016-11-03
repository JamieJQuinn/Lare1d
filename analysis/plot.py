import sys
import numpy as np
import matplotlib.pyplot as plt


def main():
    N = 300

    var = np.fromfile(sys.argv[1], dtype=np.dtype(np.double))
    density = var[:N]
    pressure = var[N:2*N]
    velocity = var[2*N:]

    f, axarr = plt.subplots(3, sharex=True)

    axarr[0].plot(density)
    axarr[1].plot(velocity)
    axarr[2].plot(pressure)

    plt.show()

main()

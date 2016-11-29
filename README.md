# Lare1D

---

This is an implementation of the LARE (LAgrangian-REmap) scheme in 1D applied to the inviscid Euler equations of fluid dynamics. The simulation is implemented in C++.

## Building and Running Unit Tests
To build, run `make` in the top level directory. The build files will be placed in `build`. 

To run unit tests, run `make test`.

## Running Lare1D
This code is entirely data driven, that is there are no parameters necessarily passed to the command when the simulation runs (although there are options availabe). Neither are the initial conditions and parameters hard-coded into the simulation. When the program is run without any command line parameters, it looks for 2 files in the working directory:
- a `constants.json` file to give simulation parameters,
- an `initial_conditions.dat` file, which specifies the initial state of the fluid system. 

## Input/Output
All data files produced by the tools and the simulation are binary files consisting of three c-style arrays arranged contiguously, each of length given by the number of grid points. The variable type of the array is given in `include/precision.hpp` and is by default `double`. The arrays are writted in the order
- pressure,
- density,
- velocity.

## Sample Simulations
The sample simulations so far are
* Riemann Problem

This can be run by running the `run_example.sh` script in the `example` folder using `sh run_example.sh`.

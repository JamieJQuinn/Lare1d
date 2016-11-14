#!/usr/bin/env bash

cd data/RiemannData
rm *.dat
../../lare1d
../../analysis/plot.py -c show -N 800 --over=analyticalSoln.dat  RiemannSoln.dat
cd ../..

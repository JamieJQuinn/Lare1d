#!/usr/bin/env bash

cd data/RiemannData
rm *.dat
../../lare1d
../../analysis/plot.py -c show -N 300 --over=analyticalSoln324.dat  RiemannSoln324.dat
cd ../..

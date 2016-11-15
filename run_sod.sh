#!/usr/bin/env bash

mkdir -p data/SodData
cd data/SodData
rm *.dat
../../lare1d
../../analysis/plot.py -c show -N 500 SodSoln.dat
cd ../..

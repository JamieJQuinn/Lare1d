N_GRID_POINTS=500
N_TIMESTEPS=350

echo '{"dt":0.0001,"gamma":2,"nGridPoints":'$N_GRID_POINTS',"nTimeSteps":'$N_TIMESTEPS',"sigma1":0.1,"sigma2":0.5}' > constants.json
../tools/makeAnalyticalSolution.py -N $N_GRID_POINTS -o analytical$N_TIMESTEPS.dat -t 0.0$N_TIMESTEPS riemann
../tools/makeICFile.py -N $N_GRID_POINTS riemann
../build/lare1d
../tools/plot.py -N $N_GRID_POINTS -c show numerical$N_TIMESTEPS.dat --over analytical$N_TIMESTEPS.dat

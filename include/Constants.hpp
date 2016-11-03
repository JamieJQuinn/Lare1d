#pragma once

#include <Precision.hpp>

class Constants {
  public:
    real dt;
    real gamma;
    int nGridPoints;
    int nTimeSteps;
    real sigma1;
    real sigma2;

    Constants(real dtIn, real gammaIn, int nGridPointsIn, int nTimeStepsIn, real sigma1In, real sigma2In);
};

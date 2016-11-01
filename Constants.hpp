#pragma once

#include "Precision.hpp"

class Constants {
  public:
    real dt;
    real gamma;
    real nGridPoints;
    real sigma1;
    real sigma2;

    Constants(real dtIn, real gammaIn, real nGridPointsIn, real sigma1In, real sigma2In);
};

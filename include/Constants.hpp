#pragma once

#include <Precision.hpp>
#include <json.hpp>
#include <string>

class Constants {
  public:
    real dt;
    real gamma;
    int nGridPoints;
    int nTimeSteps;
    real sigma1;
    real sigma2;

    Constants();
    Constants(real dtIn, real gammaIn, int nGridPointsIn, int nTimeStepsIn, real sigma1In, real sigma2In);
    void readJson(const std::string filePath);
    void writeJson(const std::string filePath) const;
};

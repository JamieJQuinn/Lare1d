#include <Constants.hpp>
#include <fstream>
#include <iostream>

Constants::Constants() {}

Constants::Constants(real dtIn, real gammaIn, int nGridPointsIn, int nTimeStepsIn, real sigma1In, real sigma2In):
  dt(dtIn),
  gamma(gammaIn),
  nGridPoints(nGridPointsIn),
  nTimeSteps(nTimeStepsIn),
  sigma1(sigma1In),
  sigma2(sigma2In)
{}

void Constants::readJson(const std::string filePath) {
  nlohmann::json j;

  std::ifstream in(filePath);
  if(in.is_open()) {
    in >> j;
    in.close();
  } else {
    std::cerr << "Could not read constants from JSON file " << filePath << std::endl;
  }
  
  dt = j["dt"];
  gamma = j["gamma"];
  nGridPoints = j["nGridPoints"];
  nTimeSteps = j["nTimeSteps"];
  sigma1 = j["sigma1"];
  sigma2 = j["sigma2"];
}

void Constants::writeJson(const std::string filePath) const {
  nlohmann::json j;

  j["dt"] = dt;
  j["gamma"] = gamma;
  j["nGridPoints"] = nGridPoints;
  j["nTimeSteps"] = nTimeSteps;
  j["sigma1"] = sigma1;
  j["sigma2"] = sigma2;

  std::ofstream out(filePath);
  if (out.is_open()) {
    out << j;
    out.close();
  } else {
    std::cerr << "Could not print constants to JSON file " << filePath << std::endl;
  }
}

#include <ModelVariables.hpp>
#include <Precision.hpp>
#include <iostream>
#include <cstdio>
#include <string>

ModelVariables::ModelVariables(const Constants& c):
  density(c.nGridPoints, 2),
  pressure(c.nGridPoints, 1), // Pressure derived from other variables, only ever need one cache
  velocity(c.nGridPoints, 3),
  dxb(c.nGridPoints, 2),
  dxc(c.nGridPoints, 2),
  energy(c.nGridPoints, 2),
  dM(c.nGridPoints, 1),
  de(c.nGridPoints, 1),
  du(c.nGridPoints, 1),
  length(c.nGridPoints)
{
  // Initialise grid and energy
  real* dxcData = dxc.get();
  real* dxbData = dxb.get();
  for(int i=0; i<c.nGridPoints*2; ++i) {
    dxcData[i] = dxbData[i] = 1.0f/real(c.nGridPoints);
  }

  vars.push_back(&pressure);
  vars.push_back(&density);
  vars.push_back(&velocity);
  vars.push_back(&dxb);
  vars.push_back(&dxc);
  vars.push_back(&energy);
  vars.push_back(&dM);
  vars.push_back(&de);
  vars.push_back(&du);

  IO.push_back(&density);
  IO.push_back(&pressure);
  IO.push_back(&velocity);
}

int ModelVariables::len() const {
  return this->length;
}

void ModelVariables::nextTimestep(int nSteps) {
  for(auto var : vars) {
    var->nextTimestep(nSteps);
  }
}

void ModelVariables::print() const {
  printTo(std::cout);
}

void ModelVariables::printTo(std::ostream& stream) const {
  for(auto var : vars) {
    var->printTo(stream);
  }
}

int ModelVariables::load(const std::string& filePath, const Constants& c) {
  FILE* inFile = fopen(filePath.c_str(), "rb");

  if( inFile == NULL ) { return -1; }

  for(auto var : IO) {
    var->readFromFile(inFile, 1);
  }

  // Calculate other required variables that aren't saved
  real* e = energy.get();
  real* dxcData = dxc.get();
  real* dxbData = dxb.get();
  for(int i=0; i<len(); ++i) {
    e[i] = pressure[i]/(density[i]*(c.gamma-1));
    dxcData[i] = dxbData[i] = 1.0f/c.nGridPoints;
  }

  fclose(inFile);

  return 0;
}

int ModelVariables::save(const std::string& filePath) const {
  FILE* outFile = fopen(filePath.c_str(), "wb");

  if( outFile == NULL ) { return -1; }

  for(auto var : IO) {
    var->writeToFile(outFile, 1);
  }

  fclose(outFile);

  return 0;
}

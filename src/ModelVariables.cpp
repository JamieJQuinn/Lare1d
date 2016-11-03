#include <ModelVariables.hpp>
#include <Precision.hpp>
#include <cstdio>
#include <string>

ModelVariables::ModelVariables(int length):
  density(length, 2),
  pressure(length, 1), // Pressure derived from other variables, only ever need one cache
  velocity(length, 3),
  dxb(length, 2),
  dxc(length, 2),
  energy(length, 2),
  dM(length, 1),
  de(length, 1),
  du(length, 1),
  length(length)
{
  vars.push_back(&density);
  vars.push_back(&pressure);
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

int ModelVariables::load(const std::string& filePath, const Constants& c) {
  FILE* inFile = fopen(filePath.c_str(), "rb");

  if( inFile == NULL ) { return -1; }

  for(auto var : IO) {
    var->readFromFile(inFile, 1);
  }

  // Calculate other required variables that aren't saved
  real* e = energy.get();
  real* dxcArray = dxc.get();
  real* dxbArray = dxb.get();
  for(int i=0; i<len(); ++i) {
    e[i] = pressure[i]/(density[i]*(c.gamma-1));
    dxcArray[i] = dxbArray[i] = 1.0f/c.nGridPoints;
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

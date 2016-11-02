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
  du(length, 1)
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
}

int ModelVariables::len() const {
  return this->length;
}

void ModelVariables::nextTimestep(int nSteps) {
  for(auto var : vars) {
    var->nextTimestep(nSteps);
  }
}

int ModelVariables::load(const std::string filePath) {
  FILE* inFile = fopen(filePath.c_str(), "rb");

  if( inFile == NULL ) { return -1; }

  for(auto var : vars) {
    var->readFromFile(inFile);
  }

  fclose(inFile);

  return 0;
}

int ModelVariables::save(const std::string filePath) const {
  FILE* outFile = fopen(filePath.c_str(), "wb");

  if( outFile == NULL ) { return -1; }

  for(auto var : vars) {
    var->writeToFile(outFile);
  }

  fclose(outFile);

  return 0;
}

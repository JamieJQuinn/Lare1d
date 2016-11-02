#include <ModelVariables.hpp>
#include <Precision.hpp>
#include <cstdio>
#include <string>

ModelVariables::ModelVariables(int length):
  density(length, 2),
  pressure(length, 1),
  velocity(length, 2),
  dxb(length, 2),
  dxc(length, 2),
  energy(length, 2),
  dM(length, 1)
{
  vars.push_back(&density);
  vars.push_back(&pressure);
  vars.push_back(&velocity);
  vars.push_back(&dxb);
  vars.push_back(&dxc);
  vars.push_back(&energy);
  vars.push_back(&dM);
}

int ModelVariables::len() const {
  return this->length;
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

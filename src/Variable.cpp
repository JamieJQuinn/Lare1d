#include <Variable.hpp>
#include <Precision.hpp>
#include <iostream>

real* Variable::get() const {
  return getPlus(0);
}

real* Variable::getPlus(int nSteps) const {
  return data + ((current+nSteps)%totalSteps)*len();
}

void Variable::nextTimestep(int nSteps) {
  current += nSteps;
  current %= totalSteps;
}

int Variable::readFromFile(FILE* fp, int nSteps) {
  int totalLength = len()*nSteps;

  // This makes sure data is read with current step first
  size_t nItemsRead = 0;
  for(int i=0; i<nSteps; ++i) {
    nItemsRead += std::fread(getPlus(i), sizeof(real), len(), fp);
  }

  if( int(nItemsRead) != int(sizeof(real))*totalLength ) {
    return -1;
  } else {
    return 0;
  }
}

int Variable::writeToFile(FILE* fp, int nSteps) const {
  int totalLength = len()*nSteps;

  // This makes sure data is written with current step first
  size_t nItemsWritten = 0;
  for(int i=0; i<nSteps; ++i) {
    nItemsWritten += std::fwrite(getPlus(i), sizeof(real), len(), fp);
  }

  if( int(nItemsWritten) != totalLength ) {
    return -1;
  } else {
    return 0;
  }
}

int Variable::writeToFile(FILE* fp) const {
  return writeToFile(fp, totalSteps);
}
  
int Variable::readFromFile(FILE* fp) {
  return readFromFile(fp, totalSteps);
}

int Variable::len() const {
  return this->length;
}

void Variable::printTo(std::ostream& stream) const {
  for(int i = 0; i < len(); ++i) {
    stream << (*this)[i] << ", ";
  }
  stream << std::endl;
}

real Variable::operator[](const int i) const {
  return data[current * len() + i];
}

Variable::Variable(int inLength) {
  Variable(inLength, 1);
}

Variable::Variable(int inLength, int inTotalSteps):
  data(new real[inLength*inTotalSteps]),
  length(inLength),
  totalSteps(inTotalSteps),
  current(0)
{}

Variable::~Variable() {
  delete [] data;
}

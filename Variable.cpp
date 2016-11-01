#include "Variable.hpp"
#include "Precision.hpp"
#include <iostream>

real* Variable::get() {
  return getPlus(0);
}

real* Variable::getPlus(int nSteps) {
  return data + (current+nSteps)%totalSteps;
}

int Variable::readFromFile(FILE* fp) {
  int totalLength = len()*totalSteps;
  size_t nItemsRead = std::fread(data, sizeof(real), totalLength, fp);

  if( int(nItemsRead) != int(sizeof(real))*totalLength ) {
    return -1;
  } else {
    return 0;
  }
}

int Variable::writeToFile(FILE* fp) const {
  int totalLength = len()*totalSteps;
  size_t nItemsWritten = std::fwrite(data, sizeof(real), totalLength, fp);

  if( int(nItemsWritten) != totalLength ) {
    return -1;
  } else {
    return 0;
  }
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

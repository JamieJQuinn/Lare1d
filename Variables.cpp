#include "Variables.hpp"
#include "Precision.hpp"
#include <cstdio>
#include <string>

int Variables::writeArray(const real *array, FILE* fp) const {
  size_t nItemsWritten = std::fwrite(array, sizeof(real), this->length(), fp);

  if( int(nItemsWritten) != this->length() ) {
    return -1;
  } else {
    return 0;
  }
}

int Variables::readArray(real *array, FILE* fp) {
  size_t nItemsRead = std::fread(array, sizeof(real), this->length(), fp);

  if( int(nItemsRead) != int(sizeof(real))*this->length() ) {
    return -1;
  } else {
    return 0;
  }
}

int Variables::load(const std::string filePath) {
  FILE* inFile = fopen(filePath.c_str(), "rb");

  if( inFile == NULL ) { return -1; }

  int error = 0;
  if( readArray(density, inFile) != 0 ) {
    error = -1;
  }
  if( readArray(pressure, inFile) != 0 ) {
    error = -1;
  }
  if( readArray(velocity, inFile) != 0 ) {
    error = -1;
  }

  fclose(inFile);

  return error;
  
}

int Variables::save(const std::string filePath) const {
  FILE* outFile = fopen(filePath.c_str(), "wb");

  if( outFile == NULL ) { return -1; }

  int error = 0;
  if( writeArray(density, outFile) != 0 ) {
    error = -1;
  }
  if( writeArray(pressure, outFile) != 0 ) {
    error = -1;
  }
  if( writeArray(velocity, outFile) != 0 ) {
    error = -1;
  }

  fclose(outFile);

  return error;
}

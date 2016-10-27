#include <stdio.h>
#include <string>

// Presicion
typedef double real;

class Variables {
  real *density, *pressure, *velocity;
  int len;

  private:
    int writeArray(real*, FILE*);

  public:
    int save(std::string);
    int length() {return this->len;}
};

int Variables::writeArray(real *array, FILE* fp) {
  size_t nItemsWritten = fwrite(array, sizeof(real), this->length(), fp);

  if( int(nItemsWritten) != this->length() ) { return -1; }
  return 0;
}

int Variables::save(std::string filePath) {
  FILE* oFile = fopen(filePath.c_str(), "wb");

  if( oFile == NULL ) { return -1; }

  int error = 0;
  if( writeArray(density, oFile) != 0 ) {
    error = -1;
  }
  if( writeArray(pressure, oFile) != 0 ) {
    error = -1;
  }
  if( writeArray(velocity, oFile) != 0 ) {
    error = -1;
  }

  fclose(oFile);

  return error;
}

int main(int argc, char** argv) {
  return 0;
}

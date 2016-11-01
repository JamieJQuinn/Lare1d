#pragma once

#include "Precision.hpp"
#include <cstdio>
#include <string>

class Variables {
  real *density, *pressure, *velocity;
  int len;

  private:
    int writeArray(const real*, FILE*) const;
    int readArray(real*, FILE*);

  public:
    int save(const std::string) const;
    int load(const std::string);
    int length() const {return this->len;}
};

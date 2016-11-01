#pragma once

#include "Precision.hpp"
#include "Variable.hpp"
#include <cstdio>
#include <string>
#include <vector>

class ModelVariables {
  public:
    Variable density, pressure, velocity, dxb, dxc, energy, dM;

    int save(const std::string) const;
    int load(const std::string);
    int len() const;

    ModelVariables(int length);

  private:
    std::vector<Variable*> vars;
    int length;
};

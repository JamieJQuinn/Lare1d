#pragma once

#include "Precision.hpp"
#include "Variable.hpp"
#include <cstdio>
#include <string>
#include <vector>

class ModelVariables {
  public:
    int save(const std::string) const;
    int load(const std::string);
    int len() const;

    ModelVariables(int length);
    ~ModelVariables();

  private:
    Variable density, pressure, velocity, dxb, dxc, energy, dM;
    std::vector<Variable*> vars;
    int length;
};

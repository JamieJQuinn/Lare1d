#pragma once

#include <Precision.hpp>
#include <Variable.hpp>
#include <cstdio>
#include <string>
#include <vector>

class ModelVariables {
  public:
    Variable density,
             pressure,
             velocity,
             dxb,
             dxc,
             energy,
             dM,
             de,
             du;

    int save(const std::string) const;
    int load(const std::string);
    int len() const;
    void nextTimestep(int nSteps=1);

    ModelVariables(int length);

  private:
    std::vector<Variable*> vars;
    int length;
};

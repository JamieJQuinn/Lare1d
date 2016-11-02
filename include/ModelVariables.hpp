#pragma once

#include <Precision.hpp>
#include <Variable.hpp>
#include <Constants.hpp>
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
    int load(const std::string, const Constants c);
    int len() const;
    void nextTimestep(int nSteps=1);

    ModelVariables(int length);

  private:
    // List of all vars
    std::vector<Variable*> vars;
    // List of IO vars
    std::vector<Variable*> IO;
    int length;
};

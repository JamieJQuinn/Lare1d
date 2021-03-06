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
             dxBound,
             dxCell,
             energy;

    int save(const std::string& filePath) const;
    int load(const std::string& filePath, const Constants& c);
    int len() const;
    void nextTimestep(int nSteps=1);
    void print() const;
    void printTo(std::ostream& stream) const;

    ModelVariables(const Constants& c);

  private:
    // List of all vars
    std::vector<Variable*> vars;
    // List of IO vars
    std::vector<Variable*> IO;
    int length;
};

#pragma once

#include <ModelVariables.hpp>
#include <Constants.hpp>

namespace SodProblem {
  void setupInitialConditions(ModelVariables &vars, const Constants &c);

  void setupAnalyticalSolution(ModelVariables &vars, const Constants &c);
}

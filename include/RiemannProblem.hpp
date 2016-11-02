#pragma once

#include <ModelVariables.hpp>
#include <Constants.hpp>

void setupInitialConditions(ModelVariables &vars, const Constants &c);

void setupAnalyticalSolution(ModelVariables &vars, const Constants &c, real t);

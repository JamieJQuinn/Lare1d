#pragma once

#include <Precision.hpp>
#include <cstdio>
#include <iostream>

class Variable {
  // Encapsulates an array representing a variable in the model
  public:
    // get current array
    real * get();
    // get array representing the variable nSteps steps in the future
    real * getPlus(int nSteps);
    int readFromFile(FILE* fp);
    int writeToFile(FILE* fp) const;
    int len() const;
    real operator[](const int i) const;
    void printTo(std::ostream& stream) const;

    Variable(int inLength);
    // totalSteps gives the number of arrays to store, including the current one
    Variable(int inLength, int inTotalSteps);
    ~Variable();
    
  private:
    real * data;
    int length;
    int totalSteps;
    int current; // index pointing to slice of array representing current time
};

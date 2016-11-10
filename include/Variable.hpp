#pragma once

#include <Precision.hpp>
#include <cstdio>
#include <iostream>

class Variable {
  // Encapsulates an array representing a variable in the model
  public:
    // get current array
    real * get() const;
    // get array representing the variable nSteps steps in the future
    real * getPlus(int nSteps) const;
    int readFromFile(FILE* fp, int nSteps);
    int writeToFile(FILE* fp, int nSteps) const;
    int readFromFile(FILE* fp);
    int writeToFile(FILE* fp) const;
    int len() const;
    real operator[](const int i) const;
    void print() const;
    void printTo(std::ostream& stream) const;
    void nextTimestep(int nSteps=1);

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

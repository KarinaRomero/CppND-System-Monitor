#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 std::vector<std::string> currentData;
 std::vector<std::string> lastData;
 float CalculateIdle(std::vector<std::string> data);
 float CalculateNonIdle(std::vector<std::string> data);
};

#endif
#include "processor.h"
#include "linux_parser.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    currentData = LinuxParser::CpuUtilization();

    if(lastData.empty()) {
        lastData = currentData;
        return 0.0;
    }

    float last_idle = CalculateIdle(lastData);
    float last_no_idle = CalculateIdle(lastData); 
    float last_total = last_idle + last_no_idle;

    float current_idle = CalculateIdle(currentData);
    float current_no_idle = CalculateIdle(currentData); 
    float current_total = current_idle + current_no_idle;
    
    float total = current_total - last_total;
    float idle = current_idle - last_idle;

    lastData = currentData;

    return (total-idle) / total; 
}

float Processor::CalculateIdle(std::vector<std::string> data){
    return std::stof(data[LinuxParser::CPUStates::kIdle_]) + std::stof(data[LinuxParser::CPUStates::kIOwait_]);
}
float Processor::CalculateNonIdle(std::vector<std::string> data){
    return std::stof(data[LinuxParser::CPUStates::kUser_]) + std::stof(data[LinuxParser::CPUStates::kNice_])
            + std::stof(data[LinuxParser::CPUStates::kSystem_]) + std::stof(data[LinuxParser::CPUStates::kIRQ_])
            + std::stof(data[LinuxParser::CPUStates::kSoftIRQ_]) + std::stof(data[LinuxParser::CPUStates::kSteal_]);
}
#include "processor.h"
#include "linux_parser.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    long current_idle = LinuxParser::IdleJiffies();
    long current_non_idle = LinuxParser::ActiveJiffies();

    float last_total = last_idle + last_non_idle;
    float current_total = current_idle + current_non_idle;
    
    float total = current_total - last_total;
    float idle = current_idle - last_idle;

    last_idle = current_idle;
    last_non_idle = current_non_idle;

    return (total-idle) / total; 
}

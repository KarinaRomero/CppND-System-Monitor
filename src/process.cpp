#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include "linux_parser.h"

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    long current_system_jiffies = LinuxParser::Jiffies();
    float current_process_jiffies = LinuxParser::ActiveJiffies(pid_);
    
    if(last_system_jiffies == 0 && last_process_jiffies == 0) {
        last_process_jiffies = current_process_jiffies;
        last_system_jiffies = current_system_jiffies;
        return 0.0;
    }
    
    CurrentCpuUsage = (current_process_jiffies - last_process_jiffies) / (current_system_jiffies - last_system_jiffies);
    return CurrentCpuUsage;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() {
    string ram = LinuxParser::Ram(pid_);

    if(ram == "" || ram == " ")
     return ram_value;

    ram_value = std::to_string((std::stof(ram)) / 1024);
    
    return ram_value; 
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_);}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return a.CurrentCpuUsage < CurrentCpuUsage; }
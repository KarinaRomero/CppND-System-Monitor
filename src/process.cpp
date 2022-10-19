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
    float hertz = (float)sysconf(_SC_CLK_TCK);
    float system_uptime = LinuxParser::UpTime() / hertz;
    float active_jiffies = LinuxParser::ActiveJiffies(pid_) / hertz;
    float process_uptime = LinuxParser::UpTime(pid_) / hertz;
    
    CurrentCpuUsage = active_jiffies / (system_uptime - process_uptime);
    return CurrentCpuUsage;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() {
    float used_ram = (std::stof(LinuxParser::Ram(pid_))) / 1024;
    return std::to_string(used_ram); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_);}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return a.CurrentCpuUsage < CurrentCpuUsage; }
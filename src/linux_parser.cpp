#include <dirent.h>
#include <unistd.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::cout;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memory_total = 0;
  float memory_free = 0;
  string line;
  string prefix_mem_total = "MemTotal:";
  string prefix_mem_free = "MemFree:";

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix_mem_total, 0) == 0) {
        memory_total = std::stoi(line.substr((prefix_mem_total.length()), line.length()-2), nullptr);
      }
      else if (line.rfind(prefix_mem_free, 0) == 0) {
        memory_free = std::stoi(line.substr((prefix_mem_free.length()), line.length()-2), nullptr);
      }
    }
  }
  return  (memory_total-memory_free)/memory_total; 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  long up_time = 0;
  string line;
  int index;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    index = line.find(' ');

    up_time += std::stol(line.substr(0, index), nullptr);
  }
  return up_time; 
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK); 
}

// DONE: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string kpid = "/" + std::to_string(pid);
  string line;
  int counter = 1;
  int from = 14;
  int to = 17;
  long utime, stime,  cutime, cstime, seconds;

  std::ifstream filestream(kProcDirectory + kpid + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    
    std::stringstream streamLine(line);
    std::string string_value;
    
    while (std::getline(streamLine, string_value, ' ')) {
      if(counter >= from && counter <= to) {
        if(counter == 14)
          utime = std::stol(string_value);
        if(counter == 15)
          stime = std::stol(string_value);
        if(counter == 16)
          cutime = std::stol(string_value);
        if(counter == 17)
          cstime = std::stol(string_value);
      } 
      counter++;
    }
  }
  seconds = utime + stime + cutime + cstime;
  return seconds;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto data = CpuUtilization();
  
  return std::stol(data[LinuxParser::CPUStates::kUser_]) + std::stol(data[LinuxParser::CPUStates::kNice_])
            + std::stol(data[LinuxParser::CPUStates::kSystem_]) + std::stol(data[LinuxParser::CPUStates::kIRQ_])
            + std::stol(data[LinuxParser::CPUStates::kSoftIRQ_]) + std::stol(data[LinuxParser::CPUStates::kSteal_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto data = CpuUtilization();
  
  return std::stol(data[LinuxParser::CPUStates::kIdle_]) + std::stol(data[LinuxParser::CPUStates::kIOwait_]);
  
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpu_data{};
  string line;

  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::stringstream streamLine(line);
    std::string string_value;
    
    while (std::getline(streamLine, string_value, ' '))
      if(string_value!="" && string_value != "cpu"&& string_value != " ")
        cpu_data.push_back(string_value);
  }
  return cpu_data; 
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int total_processes;
  string line;
  string prefix = "processes ";

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix, 0) == 0) {
        total_processes = std::stoi(line.substr((prefix.length()), line.length()), nullptr);
      }
    }
  }
  return total_processes; 
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int total_processes_running;
  string line;
  string prefix = "procs_running ";

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix, 0) == 0) {
        total_processes_running = std::stoi(line.substr((prefix.length()), line.length()), nullptr);
      }
    }
  }
  return total_processes_running;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string kpid = "/" + std::to_string(pid);
  string line;

  std::ifstream filestream(kProcDirectory + kpid + kCmdlineFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
  }

  return line;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string pid_path = "/"+ std::to_string(pid);
  string size;
  string line; 
  string prefix = "VmSize:";

  std::ifstream filestream(kProcDirectory + pid_path + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix, 0) == 0) {

        size = line.substr(prefix.length(), line.length());
        size.erase(remove(size.begin(), size.end(), '\t'));
        break;
      }
    }
  }
  return size;
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string pid_path = "/"+ std::to_string(pid);
  string uid;
  string line;
  string prefix = "Uid:\t";

  std::ifstream filestream(kProcDirectory + pid_path + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix, 0) == 0) {

        int index = (line.substr(prefix.length(), line.length())).find('\t');
        uid = line.substr((prefix.length()), index);
        break;
      }
    }
  }
  return uid; 
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, user;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {

      if (line.find(":"+uid+":") != string::npos) {
        int index = (line.substr(0, line.length())).find(':');
        user = line.substr(0, index);
        break;
      }
    }
  }
    
  return user;
}

// DONE: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string kpid = "/" + std::to_string(pid);
  string line;
  int counter = 1;
  long up_time = 0;

  std::ifstream filestream(kProcDirectory + kpid + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    
    std::stringstream streamLine(line);
    std::string string_value;
    
    while (std::getline(streamLine, string_value, ' ')) {
      if(14 == counter || 15 == counter) {
        up_time += std::stol(string_value);
      } 
      counter++;
    }

  }
  return up_time;
}

bool LinuxParser::ProcessFileExist(int pid) {
  string kpid = "/" + std::to_string(pid);
  std::ifstream filestream(kProcDirectory + kpid + kStatFilename);
  if(filestream.is_open()) {
    return true;
  }
  return false;
}
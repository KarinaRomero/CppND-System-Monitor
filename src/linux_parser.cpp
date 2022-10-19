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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memory_total = 0;
  float memory_free = 0;
  string line;
  string prefixMemTotal = "MemTotal:";
  string prefixMemFree = "MemFree:";

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefixMemTotal, 0) == 0) {
        memory_total = std::stoi(line.substr((prefixMemTotal.length()), line.length()-2), nullptr);
      }
      else if (line.rfind(prefixMemFree, 0) == 0) {
        memory_free = std::stoi(line.substr((prefixMemTotal.length()), line.length()-2), nullptr);
      }
    }
  }
  return  (memory_total-memory_free)/memory_total; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long up_time = 0;
  string line;
  int index;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    index = line.find(' ');

    up_time += std::stol(line.substr(0, index), nullptr);
    // cout << line << " | "  << index << " | " << up_time << "\n";
  }
  return up_time; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string kpid = "/" + std::to_string(pid);
  string line;
  int counter = 1;
  int from = 14;
  int to = 17;
  long utime, stime, cutime, cstime, starttime, system_uptime;

  std::ifstream filestream(kProcDirectory + kpid + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    // cout << line << "\n";
    
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
  system_uptime = LinuxParser::UpTime();
  starttime = LinuxParser::UpTime(pid) * sysconf(_SC_CLK_TCK);

  long total_time = utime + stime + cutime + cstime;
  long seconds = system_uptime - (starttime/sysconf(_SC_CLK_TCK));

  return 0;
  return (total_time / sysconf(_SC_CLK_TCK)) / seconds;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
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

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int totalProcesses;
  string line;
  string prefix = "processes ";

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix, 0) == 0) {
        totalProcesses = std::stoi(line.substr((prefix.length()), line.length()), nullptr);
        // cout << totalProcesses << "\n";
      }
    }
  }
  return totalProcesses; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int totalProcessesRunning;
  string line;
  string prefix = "procs_running ";

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.rfind(prefix, 0) == 0) {
        totalProcessesRunning = std::stoi(line.substr((prefix.length()), line.length()), nullptr);
        // cout << totalProcesses << "\n";
      }
    }
  }
  return totalProcessesRunning;
}

// TODO: Read and return the command associated with a process
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

// TODO: Read and return the memory used by a process
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
        
        //cout << size << "\n";
        break;
      }
    }
  }
  return size;
}

// TODO: Read and return the user ID associated with a process
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
        //cout << uid << "\n";
        break;
      }
    }
  }
  return uid; 
}

// TODO: Read and return the user associated with a process
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
        // cout << user << "\n";
        break;
      }
    }
  }
    
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string kpid = "/" + std::to_string(pid);
  string line;
  int counter = 1;
  int keynumber = 22;
  long up_time;

  std::ifstream filestream(kProcDirectory + kpid + kStatFilename);
  if(filestream.is_open()) {
    std::getline(filestream, line);
    // cout << line << "\n";
    
    std::stringstream streamLine(line);
    std::string string_value;
    
    while (std::getline(streamLine, string_value, ' ')) {
      if(keynumber == counter) {
        // cout << string_value << counter<< "\n";
        up_time = std::stol(string_value) / sysconf(_SC_CLK_TCK);
      } 
      counter++;
    }

  }
  return up_time;
}

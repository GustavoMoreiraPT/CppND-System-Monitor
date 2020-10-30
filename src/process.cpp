#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  this->_pid = pid;
}

// TODO: Return this process's ID
int Process::Pid() { return _pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  long totalSystemJiffies = LinuxParser::ActiveJiffies();
  long systemUptime = LinuxParser::UpTime();
  float pidUptime = LinuxParser::UpTime(this->Pid());
  float seconds = (systemUptime - pidUptime) / sysconf(_SC_CLK_TCK);

  return (totalSystemJiffies / sysconf(_SC_CLK_TCK) / seconds);
}

// TODO: Return the command that generated this process
string Process::Command() { 
	return LinuxParser::Command(this->Pid());
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
	return LinuxParser::Ram(this->Pid());
}

// TODO: Return the user (name) that generated this process
string Process::User() {
	return LinuxParser::User(this->Pid());
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
	return LinuxParser::UpTime(this->Pid());
}

bool Process::operator<(Process const& a) const {
  	return LinuxParser::convertValues(LinuxParser::Ram(this->_pid)) < LinuxParser::convertValues(LinuxParser::Ram(a._pid));
}
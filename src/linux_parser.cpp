#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

//Implementation taken from here:
//https://stackoverflow.com/questions/8362094/replace-multiple-spaces-with-one-space-in-a-string
bool LinuxParser::BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

template <typename T>
T LinuxParser::findValueByKey(string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

template <typename T>
T LinuxParser::getValueOfFile(string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};

long LinuxParser::convertValues(string const &value){
  	long convertedValues = 0;
	try{
      convertedValues = std::stol(value);
    }
    catch(...){
      // do nothing for now
    }
  
  	return convertedValues;
}

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

float LinuxParser::MemoryUtilization() { 
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}

long LinuxParser::UpTime() {
  string uptime;
  string key;
  string value;
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::string delimiter = " ";
	  uptime = line.substr(0, line.find(delimiter));
    }
  }
  
  long parsedUptime = atol(uptime.c_str());
  
  return parsedUptime;
}

// Implemantation based in this knowledge question https://knowledge.udacity.com/questions/129844, couldn't really understand it from the classes
long LinuxParser::Jiffies() { 
  long uptime = LinuxParser::UpTime();
  auto ticks = sysconf (_SC_CLK_TCK);
  
  return uptime * ticks;
}

//Implementation based in this knowledge question https://knowledge.udacity.com/questions/129844, couldn't really understand it from the classes
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  long pidJiffies = 0;
  
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if(file.is_open()){
    std::getline(file, line);
    std::istringstream stream(line);
    
    for(int i=1; i<18; i++){
      stream >> value;
      
      if(14 <= i && i <= 17){
        pidJiffies = pidJiffies + std::stoi(value);
      }
    }
  }
  return pidJiffies; 
}

//Implementation based in this knowledge question https://knowledge.udacity.com/questions/129844, couldn't really understand it from the classes
long LinuxParser::ActiveJiffies() {
  auto values = LinuxParser::CpuUtilization();
  
  auto sumJiffies = 0;
  for(unsigned int i = 0; i < values.size(); i++){
    long convertedValues = LinuxParser::convertValues(values[i]);
    sumJiffies = sumJiffies + convertedValues;
  }
  
  return sumJiffies;
}

//Implementation based in this knowledge question https://knowledge.udacity.com/questions/129844, couldn't really understand it from the classes
long LinuxParser::IdleJiffies() { 
  auto values = LinuxParser::CpuUtilization();
  
  auto sumIdleJiffies = 0;
  for(unsigned int i = 0; i < values.size(); i++){
    if(i == 3 || i == 4)
    sumIdleJiffies = sumIdleJiffies + LinuxParser::convertValues(values[i]);
  }
  
  return sumIdleJiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  string line, key, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      
      while (linestream >> key) {
        if (key == "cpu") {
          linestream >> value;
          values.push_back(value);
        }
      }
    }
  }
  return values;
}

int LinuxParser::ProcessExtractor(string valueToExtract){
  string line, key;
  int value = 0;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == valueToExtract) {
          return value;
        }
      }
    }
  }
  return value;
}

int LinuxParser::TotalProcesses() { 
  return LinuxParser::ProcessExtractor("processes");
}

int LinuxParser::RunningProcesses() { 
  return LinuxParser::ProcessExtractor("procs_running");
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  string line, key;
  float value = 0.0;
  string defaultValue = "";
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          std::stringstream secondStream;
          secondStream << value;
          std::string parsedValue = secondStream.str();
          int assertRamToMegabytes = static_cast<int>(std::stof(parsedValue) * 0.01);
          
          return to_string(assertRamToMegabytes);
        }
      }
    }
  }
  return defaultValue;
}

string LinuxParser::Uid(int pid) {
  string uid = "Uid:";
  string uidPath = kProcDirectory + std::to_string(pid) + kStatusFilename;
  string valueUid = findValueByKey<string>(uid, uidPath);
  return valueUid;
}

string LinuxParser::User(int pid) { 
  string username;
  string key;
  string uid = LinuxParser::Uid(pid);
  string foundUid;
  string line;
  std::ifstream filestream(kPasswordPath);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::string delimiter = ":";
	  username = line.substr(0, line.find(delimiter));
      foundUid = line.substr(2, line.find(delimiter));
      if(uid == foundUid){
      	return username;
      }
    }
  }
  return username;
}

long LinuxParser::UpTime(int pid) {
  std::string line, value;
  
  long time = 0;
  string noUse;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream stream(line);
    for(unsigned int i = 1; i < 22; i++) {
      stream >> noUse;
    }
    stream >> time;
  }
  filestream.close();
  return UpTime() - time / sysconf(_SC_CLK_TCK);
}
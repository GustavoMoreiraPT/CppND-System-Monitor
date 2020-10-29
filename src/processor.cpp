#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  
	float const activeJiffies = static_cast<float>(LinuxParser::ActiveJiffies());
    float const totalJiffies =  static_cast<float>(LinuxParser::Jiffies());
    return activeJiffies / totalJiffies; 
}
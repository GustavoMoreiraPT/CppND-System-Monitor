#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  
	float activeJiffies = float(LinuxParser::ActiveJiffies());
    float totalJiffies =  float(LinuxParser::Jiffies());
    return activeJiffies / totalJiffies; 
}
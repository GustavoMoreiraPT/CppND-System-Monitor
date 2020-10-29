#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) { 
	
  	const int secondsPerMinute =60;
    const int secondsPerHour =3600;
     
    long _seconds = seconds % secondsPerMinute;
    long _minutes = (seconds / secondsPerMinute) %secondsPerMinute;
    long _hours   = seconds /secondsPerHour; 
    string output = to_string(_hours) + ':' + to_string(_minutes) + ':' + to_string(_seconds);  
  
    return output;
}
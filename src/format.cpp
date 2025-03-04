#include <string>
#include <ctime>

#include "format.h"

using std::string;
using namespace std::literals::string_literals;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long int totalSeconds) { 
	tm curr_tm = {};
	char output_string[15];
    int hour, minutes, seconds;

    hour = totalSeconds / 3600;
    totalSeconds %= 3600;
    minutes = totalSeconds / 60;
    totalSeconds %= 60;
    seconds = totalSeconds;

    curr_tm.tm_sec = seconds;
    curr_tm.tm_min = minutes;
    curr_tm.tm_hour = hour;
	strftime(output_string, 15, "%T", &curr_tm);
    
    return output_string;
}
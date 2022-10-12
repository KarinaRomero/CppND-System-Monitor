#include <string>

#include "format.h"

using std::string;
using namespace std::literals::string_literals;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long int totalSeconds) { 

    totalSeconds = totalSeconds % (24 * 3600);
    int hour = totalSeconds / 3600;
     
    totalSeconds %= 3600;
    int minutes = totalSeconds / 60;
    
    totalSeconds %= 60;
    int seconds = totalSeconds;
    
    return std::to_string(hour) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
}
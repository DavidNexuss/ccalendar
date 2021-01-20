#pragma once
#include <vector>
#include <unordered_map>
#include <string>

std::string getCalendar();

struct Calendar
{
    std::unordered_map<std::string,int> color_map;
    std::vector<std::string> color_codes;
    void readColors();
    bool useConky = false;
};

extern Calendar calendar;

const static std::string color_codes_ch[] = {"\e[0;31m","\e[0;34m","\e[0;32m","\e[0;33m","\e[0;35m","\e[0;36m","\e[0;37m","\e[0;38m","\e[0;39m"};

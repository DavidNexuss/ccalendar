#pragma once
#include "Calendar.hpp"
#include <string>
#include <ctime>
#include <iostream>
std::tm make_tm(int year, int month, int day);
inline long make_timestamp(const std::tm& tm) { return tm.tm_year * 365 + tm.tm_mon * 30 + tm.tm_mday; }

struct Entry
{
    std::string name;
    std::tm tm;                                 //Saved in unix mode

    int hour,minute;

    bool hour_time = false;
    int type = 0;

    void set(int dyear,int dmonth,int dday); //set entry date in normal format

    inline void today() { type = 1; }
    inline long monthstamp() const { return tm.tm_year * 12 + tm.tm_mon; }
    inline long timestamp() const { return make_timestamp(tm); }
    inline long hourstamp() const { return hour*60 + minute; }

    inline int year() const { return tm.tm_year + 1900; }
    inline int month() const { return tm.tm_mon + 1; }
    inline int day() const { return tm.tm_mday; }

    bool operator <(const Entry& other) const {
        long a = timestamp();
        long b = other.timestamp();
        return a < b || (a == b && hourstamp() < other.hourstamp());
    }

    template<bool conky> 
    void print_hour() const
    {
        if(conky) printf(" [ ${color %s}%2.2i:%2.2i${color} ] ",calendar.color_codes[type].c_str(),hour,minute);
        else printf(" [ %s%2.2i:%2.2i%s ] ",color_codes_ch[type].c_str(),hour,minute,"\e[39m");
    }
    
    template<bool conky> 
    void print_date() const
    {
        if(conky) printf(" [ ${color %s}%2.2i/%2.2i/%2.2i${color} ] ",calendar.color_codes[type].c_str(),day(),month(),year());
        else printf(" [ %s%2.2i/%2.2i/%2.2i%s ] ",color_codes_ch[type].c_str(),day(),month(),year(),"\e[39m");

        printf("%d ",timestamp() - make_timestamp(calendar.currentDay));
    }

    template<bool conky>
    void print_entry() const
    {
        printf("  ");
        print_date<conky>();
        if (hour_time) print_hour<conky>();
        std::cout << name;
    }

    void print() const
    {
        if (calendar.useConky) print_entry<true>();
        else print_entry<false>();
    }
};

std::istream &operator >> (std::istream & is, Entry& ent);

#pragma once
#include "Calendar.hpp"
#include <string>
#include <ctime>
#include <iostream>
std::tm make_tm(int year, int month, int day);
inline long make_timestamp(const std::tm& tm) { return tm.tm_year * 365 + tm.tm_mon * 30 + tm.tm_mday; }
inline long make_month_timestamp(const std::tm& tm) { return tm.tm_year * 12 + tm.tm_mon; } 

struct Entry
{
    std::string name;
    std::tm tm;                                 //Saved in unix mode

    int hour,minute;

    bool hour_time = false;
    int type = 0;

    void set(int dyear,int dmonth,int dday); //set entry date in normal format

    inline void today() { type = 1; }
    inline long monthstamp() const { return make_month_timestamp(tm); }
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

    void print_hour(std::ostream& out) const;
    void print_date(std::ostream& out) const;
};

std::istream &operator >> (std::istream & is, Entry& ent);
std::ostream &operator << (std::ostream & os, const Entry& ent);

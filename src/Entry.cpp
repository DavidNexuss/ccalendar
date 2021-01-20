#include "Entry.hpp"
#include "Calendar.hpp"
#include <iostream>
#include <sstream>
using namespace std;

std::tm make_tm(int year, int month, int day)
{
    std::tm tm = {0};
    tm.tm_year = year - 1900; // years count from 1900
    tm.tm_mon = month - 1;    // months count from January=0
    tm.tm_mday = day;         // days count from 1
    return tm;
}
void Entry::set(int dyear,int dmonth,int dday)
{
    tm = make_tm(dyear,dmonth,dday);
}
istream &operator >> (istream & is, Entry& ent)
{

    //Set date from YYYY/MM/DD format string
    {
        string s;
        is >> s;
        long years,months,days;
        int n = s.find('/');
        years = atoi(s.substr(0,n).c_str());
        s.erase(0,n + 1);

        n = s.find('/');
        months = atoi(s.substr(0,n).c_str());
        s.erase(0,n + 1);

        days = atoi(s.c_str());
        ent.set(years,months,days);
    }


    getline(is,ent.name);
    int idx = ent.name.find("|");
    ent.hour_time = false;
    if (idx != string::npos)
    {

        stringstream ss(ent.name);
        string word;
        while(ss >> word && word != "|")
        {
            if (word == "Time")
            {
                ss >> ent.hour >> ent.minute;
                ent.hour_time = true;
            } else ent.type = calendar.color_map[word];      
        }

        ent.name.erase(0,idx + 1);
    }
    else ent.type = 0;
    return is;
}

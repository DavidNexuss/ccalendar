#include "EntryTable.hpp"
#include <iostream>
#include <sstream>

using namespace std;

#define CALENDAR_SIZE 32
EntryTable::EntryTable() : important_days(CALENDAR_SIZE * 2) { }

inline int importantDaysIndex(const std::tm & tm){
    return tm.tm_mday + ( make_month_timestamp(tm) - make_month_timestamp(calendar.currentDay))*CALENDAR_SIZE;
}
void EntryTable::readEntryTable()
{
    Entry ent;
    long current = make_timestamp(calendar.currentDay);
    important_days[importantDaysIndex(calendar.currentDay)] = 1;
    while(cin >> ent)
    {
        long diff;
        if ((diff = ent.timestamp() - current) >= 0)
        {
            if (diff == 0) ent.today();

            entries.push_back(ent);
            important_days[importantDaysIndex(ent.tm)] = ent.type;
        }
    }
}

void EntryTable::printEntryTable(int i,int offset) const
{
    while(i < entries.size())
    {
        for(int j = 0; j < offset; j++) cout << " ";
        cout << entries[i++];
        cout << endl;
    }
}

void EntryTable::printCalendar(const string& cal) const
{
    stringstream stream(cal);
    string cal_line;
    int entry_index = 0;

    getline(stream, cal_line); cout << cal_line;
    if (entry_index < entries.size()) cout << entries[entry_index++];
    cout << endl;
    getline(stream, cal_line); cout << cal_line;
    if (entry_index < entries.size()) cout << entries[entry_index++];
    cout << endl;
    
    int cal_size = 0;
    while(getline(stream, cal_line))
    {
        cal_size = max(cal_size,int(cal_line.size()));
        vector<string> weeks;
        int l = 21; //Week size magic number
        for(int i = 0; i < cal_line.size(); i+=l)
        {
            weeks.push_back(cal_line.substr(i,min(l + i + 1,int(cal_line.size()))));
            if (i > 0) weeks.back().erase(0,1);
        }
        int cal = 0;
        for(string& week : weeks)
        {
            stringstream day_stream(week);
            int d,accum = 0;
            while(day_stream >> d)
            {
                int idx = d + cal*CALENDAR_SIZE;
                if (important_days[idx])
                {
                    int token = week.find(to_string(d),accum);
                    if (token != string::npos)
                    {
                        week.erase(token,to_string(d).size());
                        string data;
                        if (calendar.useConky)
                        data = "${color " + calendar.color_codes[important_days[idx]] + "}" + to_string(d) + "${color}";
                        else
                        {
                            data = color_codes_ch[important_days[idx]] + to_string(d) + "\e[39m";
                        }
                        week.insert(token,data);
                        accum = token + data.size();
                    }
                }
            }
            cal++;
            cout << week;
        }
        if (entry_index < entries.size()) cout << entries[entry_index++];
        cout << endl;
    }
    printEntryTable(entry_index,cal_size);
}

#include "EntryTable.hpp"
#include <iostream>
#include <sstream>

using namespace std;

EntryTable::EntryTable() : important_days(64) { }
void EntryTable::readEntryTable()
{
    Entry ent;
    Entry today;
    today.tm = calendar.currentDay;
    long current = today.timestamp();
    long current_mon = today.monthstamp();
    long diff;
    while(cin >> ent)
    {
        if ((diff = ent.timestamp() - current) >= 0)
        {
            if (diff == 0) ent.today();

            entries.push_back(ent);
            important_days[ent.tm.tm_mday + (ent.monthstamp() - current_mon)*32] = ent.type;
        }
    }
}

void EntryTable::printEntryTable(int i,int offset) const
{
    while(i < entries.size())
    {
        for(int j = 0; j < offset; j++) cout << " ";
        entries[i++].print();
        cout << endl;
    }
}

void EntryTable::printCalendar(const string& cal) const
{
    stringstream stream(cal);
    string cal_line;
    int entry_index = 0;

    getline(stream, cal_line); cout << cal_line;
    if (entry_index < entries.size()) entries[entry_index++].print();
    cout << endl;
    getline(stream, cal_line); cout << cal_line;
    if (entry_index < entries.size()) entries[entry_index++].print();
    cout << endl;
    
    int cal_size = 0;
    while(getline(stream, cal_line))
    {
        cal_size = max(cal_size,int(cal_line.size()));
        vector<string> weeks;
        int l = 21;
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
                int idx = d + cal*32;
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
        if (entry_index < entries.size()) entries[entry_index++].print();
        cout << endl;
    }
    printEntryTable(entry_index,cal_size);
}

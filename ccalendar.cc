#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <locale>
#include <iomanip>
#include <set>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>

using namespace std;

unordered_map<string,int> color_map;
vector<string> color_codes;
struct Entry {
    long timestamp;
    string name;
    int year,month,day;
    int hour, minute;
    int type = 0;
    bool hour_time = false;

    bool operator < (const Entry &a ) const { 
        return timestamp < a.timestamp || (
            (timestamp == a.timestamp && hour < a.hour) || (
                (hour == a.hour && minute < a.minute)
            )
        ); 
    }
};

inline long monthstamp(long year,long month)
{
    return year * 12 + month;
}

inline long timestamp(long year,long month,long day)
{
    return year * 365 + month*30 + day;
}
std::string & ltrim(std::string & str)
{
  auto it2 =  std::find_if( str.begin() , str.end() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( str.begin() , it2);
  return str;   
}

std::string & rtrim(std::string & str)
{
  auto it1 =  std::find_if( str.rbegin() , str.rend() , [](char ch){ return !std::isspace<char>(ch , std::locale::classic() ) ; } );
  str.erase( it1.base() , str.end() );
  return str;   
}
std::istream &operator >> (std::istream & is, Entry& ent)
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
    
    ent.timestamp = timestamp(years - 1900,months - 1,days);
    ent.year = years - 1900;
    ent.month = months - 1;
    ent.day = days;
    getline(is,ent.name);
    int idx = ent.name.find("|");
    ent.hour_time = false;
    if (idx != string::npos)
    {
        /*
        string type = ent.name.substr(0,idx);
        ltrim(rtrim(type));
        if (type == "")
        {
            is >> ent.hour >> ent.minute;
            ent.hour_time = true;
        }
        else ent.type = color_map[type];*/

        stringstream ss(ent.name);
        string word;
        while(ss >> word && word != "|")
        {
            if (word == "Time")
            {
                ss >> ent.hour >> ent.minute;
                ent.hour_time = true;
            } else ent.type = color_map[word];      
        }

        ent.name.erase(0,idx + 1);
    }
    else ent.type = 2;
    return is;
}
string get_calendar()
{

    int pipe_fd[2];
    pipe(pipe_fd);
    int f = fork();
    if (f == 0)
    {
        close(pipe_fd[0]);
        close(1);
        dup2(pipe_fd[1],1);
        execlp("cal","cal","-n","2",NULL);
    }
    close(pipe_fd[1]);

    int n = 0;
    char buf[256];
    string res;
    while((n = read(pipe_fd[0],buf,sizeof(buf))) > 0)
    {
        res.append(buf,n);
    }
    return res;
}
std::tm make_tm(int year, int month, int day)
{
    std::tm tm = {0};
    tm.tm_year = year; // years count from 1900
    tm.tm_mon = month;    // months count from January=0
    tm.tm_mday = day;         // days count from 1
    return tm;
}

const static string color_codes_ch[4] = {"\033[0m","\e[0;34m","\e[0;31m","\e[0;32m"};

template<bool conky> 
void print_date(const char* color,int d,int m,int y)
{
    if(conky) printf(" [ ${color %s}%2.2i/%2.2i/%2.2i${color} ] ",color,d,m,y);
    else printf(" [ %s%2.2i/%2.2i/%2.2i%s ] ",color,d,m,y,color_codes_ch[0].c_str());
}
template<bool conky> 
void print_hour(const char* color,int h,int m)
{
    if(conky) printf(" [ ${color %s}%2.2i:%2.2i${color} ] ",color,h,m);
    else printf(" [ %s%2.2i:%2.2i%s ] ",color,h,m,color_codes_ch[0].c_str());
}
template<bool conky>
inline int print_entry(int idx,const vector<Entry>& entries,const vector<int>& important_days,int dyear,int dmonth,int dday)
{

    if (idx < entries.size())
    {
        const Entry& ent = entries[idx];
        int color = ent.type;
        if (ent.day == dday) color = 1;
        /*
        int i;
        if ((i = monthstamp(ent.year,ent.month) - monthstamp(dyear,dmonth) )<= 1)
        {
            color = important_days[ent.day + i * 32];
        }
        */

        std::time_t difference;
        {

            std::tm tm1 = make_tm(ent.year,ent.month,ent.day);    // April 2nd, 2012
            std::tm tm2 = make_tm(dyear,dmonth,dday);    // February 2nd, 2003

            // Arithmetic time values.
            // On a posix system, these are seconds since 1970-01-01 00:00:00 UTC
            std::time_t time1 = std::mktime(&tm1);
            std::time_t time2 = std::mktime(&tm2);

            // Divide by the number of seconds in a day
            const int seconds_per_day = 60*60*24;
            difference = (time1 - time2) / seconds_per_day;    

        }
        int d = ent.day;
        int m = ent.month + 1;
        int y = ent.year + 1900;
        int h = ent.hour;
        int mi = ent.minute;

        
        printf("    ");
        if (conky)
        {
            print_date<true>(color_codes[color].c_str(),d,m,y);
            if (ent.hour_time) print_hour<true>(color_codes[color].c_str(),h,mi);
        }
        else
        {
            if (color > 3) color = 2;
            print_date<false>(color_codes_ch[color].c_str(),d,m,y);
            if (ent.hour_time) print_hour<false>(color_codes_ch[color].c_str(),h,mi);
        }
        
        printf("%d %s\n",difference,ent.name.c_str());
        
       /*
        if (conky)
        if (!ent.hour_time)printf("    [ ${color %s}%2.2i/%2.2i/%2.2i${color} ] %d %s\n",color_codes[color].c_str(),ent.day,ent.month + 1,ent.year + 1900,difference,ent.name.c_str());
        else
        {
            if (color > 3) color = 2;
            printf("    [ %s%2.2i/%2.2i/%2.2i%s ] %d %s\n",color_codes_ch[color].c_str(),ent.day,ent.month + 1,ent.year + 1900,color_codes_ch[0].c_str(),difference,ent.name.c_str());
        }
        */
        return idx + 1;
    }
    cout << endl;
    return idx;
}
template <bool conky>
void print(vector<Entry>& entries,string& cal,int dyear,int dmonth,int dday)
{
    vector<int> important_days(64);
    for (auto& ent : entries)
    {
        int i;
        if ((i = monthstamp(ent.year,ent.month) - monthstamp(dyear,dmonth) )<= 1)
        {
            important_days[ent.day + (i)*32] = ent.type;
        }
    }
    important_days[dday] = 1;
    stringstream ss(cal);
    string monthname;
    string year;
    string line;
    int idx = 0;


    getline(ss,line);
    cout << line;
    idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    getline(ss,line);
    cout << line;
    idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    while(getline(ss,line))
    {
        vector<string> lines;
        int l = 21;
        for (size_t i = 0; i < line.size(); i+=l)
        {
            lines.push_back(line.substr(i,min(l + i + 1,line.size())));
            if (i > 0) lines.back().erase(0,1);
        }
        int cal = 0;
        for(auto& line2: lines)
        {
            stringstream ss2(line2);
            int n;
            int accum = 0;
            while(ss2 >> n)
            {
                int idx = n + cal*32;
                if (important_days[idx])
                {
                    int token = line2.find(to_string(n),accum);
                    if (token != std::string::npos)
                    {
                        line2.erase(token,to_string(n).size());
                        string data;
                        if (conky)
                        data = "${color " + color_codes[important_days[idx]] + "}" + to_string(n) + "${color}";
                        else
                        {
                            int color = important_days[idx];
                            if (color > 3) color = 2;
                            data = color_codes_ch[color] + to_string(n) + color_codes_ch[0];
                        }
                        line2.insert(token,data);
                        accum = token + data.size();
                    }
                }
            }
            cal++;
            cout << line2;  
        }
        idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    }
    while(idx < entries.size())
    {
        cout << "                                          ";
        idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    }
}
void read_colors()
{
    color_codes.push_back("808080");
    color_codes.push_back("80ffff");
    color_codes.push_back("ff5050");
    string name,color;
    while(cin >> name && name != "END")
    {
        cin >> color;
        color_codes.push_back(color);
        color_map[name] = color_codes.size() - 1;
    }
}
int main(int argc,char* argv[])
{
    read_colors();
    bool conky = false;
    if (argc > 1 && string(argv[1]) == "-c") conky = true;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    long current = timestamp(tm.tm_year,tm.tm_mon,tm.tm_mday);
    Entry stamp;
    vector<Entry> entries;
    while(cin >> stamp)
    {
        if (stamp.timestamp >= current)
        {
            entries.push_back(Entry(stamp));
        }
    }
    string cal = get_calendar();
    if (conky)
    print<true>(entries,cal,tm.tm_year,tm.tm_mon,tm.tm_mday);
    else 
    print<false>(entries,cal,tm.tm_year,tm.tm_mon,tm.tm_mday);

    waitpid(-1,NULL,0);
}

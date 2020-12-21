#include <iostream>
#include <vector>
#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>
#include <set>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

struct Entry {
    long timestamp;
    string name;
    int year,month,day;

    bool operator < (const Entry &a ) const { return a.timestamp < a.timestamp; }
};

long timestamp(long year,long month,long day)
{
    return year * 365 + month*30 + day;
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
        execlp("cal","cal",NULL);
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

template<bool conky>
inline int print_entry(int idx,const vector<Entry>& entries,const vector<int>& important_days,int dyear,int dmonth,int dday)
{
    static string color_codes_ch[3] = {"\033[0m","\e[0;31m","\e[0;34m"};
    static string color_codes[3] = {"909090","df2445","50bbff"};
    if (idx < entries.size())
    {
        const Entry& ent = entries[idx];
        int color = 0;
        if (ent.month == dmonth && ent.year == dyear)
        {
            color = important_days[ent.day];
        }
        if (conky)
        printf("    [ ${color %s}%2.2i/%2.2i/%2.2i${color} ] %s\n",color_codes[color].c_str(),ent.day,ent.month + 1,ent.year + 1900,ent.name.c_str());
        else 
        printf("    [ %s%2.2i/%2.2i/%2.2i%s ] %s\n",color_codes_ch[color].c_str(),ent.day,ent.month + 1,ent.year + 1900,color_codes_ch[0].c_str(),ent.name.c_str());

        return idx + 1;
    }
    cout << endl;
    return idx;
}
template <bool conky>
void print(vector<Entry>& entries,string& cal,int dyear,int dmonth,int dday)
{
    vector<int> important_days(32);
    for (auto& ent : entries)
    {
        if (ent.month == dmonth && ent.year == dyear)
        {
            important_days[ent.day] = 1;
        }
    }
    important_days[dday] = 2;
    stringstream ss(cal);
    string monthname;
    string year;
    string line;
    int idx = 0;

    ss >> monthname >> year;
    cout << "    " << monthname << " " << year << "  ";
    idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    getline(ss,line);
    getline(ss,line);
    cout << "lu ma mi ju vi sá do";
    idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    string color_codes_ch[3] = {"\033[0m","\e[0;31m","\e[0;34m"};
    string color_codes[3] = {"909090","df2445","50bbff"};
    while(getline(ss,line))
    {
        stringstream ss2(line);
        int n;
        while(ss2 >> n)
        {
            if (important_days[n])
            {
                int token = line.find(to_string(n),0);
                if (token != std::string::npos)
                {
                    line.erase(token,to_string(n).size());
                    if (conky)
                    line.insert(token,"${color " + color_codes[important_days[n]] + "}" + to_string(n) + "${color}");
                    else 
                    line.insert(token,color_codes_ch[important_days[n]] + to_string(n) + color_codes_ch[0]);
                }
            }
        }
        cout << line;
        idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);  
    }
    while(idx < entries.size())
    {
        cout << "                    ";
        idx = print_entry<conky>(idx,entries,important_days,dyear,dmonth,dday);
    }
}
int main(int argc,char* argv[])
{
    /*
    int pipe_fd[2];
    pipe(pipe_fd);
    int f = fork();

    if (f == 0)
    {
        close(0);
        dup2(pipe_fd[0],0);
        execlp("cat","cat",NULL);
    }
    close(1);
    dup2(pipe_fd[1],1);*/

    bool conky = false;
    if (argc > 1 && string(argv[1]) == "-c") conky = true;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    long current = timestamp(tm.tm_year,tm.tm_mon,tm.tm_mday);
    Entry stamp;
    vector<Entry> entries;
    while(cin >> stamp)
    {
        if (stamp.timestamp > current)
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

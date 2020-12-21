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

int nthOccurrence(const std::string& str, const std::string& findMe, int nth)
{
    size_t  pos = 0;
    int     cnt = 0;

    while( cnt != nth )
    {
        pos+=1;
        pos = str.find(findMe, pos);
        if ( pos == std::string::npos )
            return -1;
        cnt++;
    }
    return pos;
}


struct Entry {
    long timestamp;
    string name;
    int year,month,day;
    int type = 0;
    bool operator < (const Entry &a ) const { return a.timestamp < a.timestamp; }
};

long timestamp(long year,long month,long day)
{
    return year * 365 + month*30 + day;
}
long monthstamp(long year,long month)
{
    return year * 12 + month;
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
    ent.type = ent.name.find("Festa") != std::string::npos;
    ent.type += 2;
    int idx = ent.name.find("|");
    ent.name.erase(0,idx + 1);
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

const static string color_codes_ch[4] = {"\033[0m","\e[0;34m","\e[0;31m","\e[0;32m"};
const static string color_codes[4] = {"909090","50bbff","df2445","80ff80"};
template<bool conky>
inline int print_entry(int idx,const vector<Entry>& entries,const vector<int>& important_days,int dyear,int dmonth,int dday)
{

    if (idx < entries.size())
    {
        const Entry& ent = entries[idx];
        int color = 0;
        int i;
        if ((i = monthstamp(ent.year,ent.month) - monthstamp(dyear,dmonth) )<= 1)
        {
            color = important_days[ent.day + i * 32];
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
                        data = color_codes_ch[important_days[idx]] + to_string(n) + color_codes_ch[0];
                        
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

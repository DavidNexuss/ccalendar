#include "Calendar.hpp"
#include <string>
#include <unistd.h>
#include <iostream>

using namespace std;
Calendar calendar;

string getCalendar()
{
    int pipe_fd[2];
    pipe(pipe_fd);
    int f = fork();
    if (f == 0)
    {
        close(pipe_fd[0]);
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
    close(pipe_fd[0]);
    return res;
}

void Calendar::readColors()
{
    color_codes.push_back("ff5050");
    color_codes.push_back("80ffff");
    string name,color;
    while(cin >> name && name != "END")
    {
        cin >> color;
        color_codes.push_back(color);
        color_map[name] = color_codes.size() - 1;
    }
}

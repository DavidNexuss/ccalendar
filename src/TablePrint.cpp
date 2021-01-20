#include "TablePrint.hpp"
#include <iostream>
#include <sstream>
#include <queue>
#include <unistd.h>

using namespace std;

void readGrid(vector<queue<string>>& grid)
{
    string curLine;
    int currentGrid = 0;
    while(getline(cin,curLine))
    {
        stringstream ss(curLine);
        string q;
        ss >> q;
        if (q == "GRID") ss >> currentGrid;
        else grid[currentGrid].push(curLine);
    }

}
void writeGrid(vector<queue<string>>& grid)
{
    bool avaible = true;
    while(avaible)
    {
        avaible = false;
        for(int i = 0; i < grid.size(); i++)
        {
            if (!grid[i].empty())
            {
                cout << grid[i].front()<< "  ";
                grid[i].pop();
                avaible = true;
            }
            cout << delimiter;
        }
        cout << endl;
    }

}

void column()
{
    int pipe_fd[2];
    pipe(pipe_fd);
    int f = fork();
    
    if (f > 0)
    {
        close(pipe_fd[1]);
        dup2(pipe_fd[0],0);
        execlp("column","column","-t","-s",delimiter_str,NULL);
    }

    close(pipe_fd[0]);
    dup2(pipe_fd[1],1);
}
void tableGridExecute()
{
    column();
    int n;
    cin >> n;
    {
        string trash;
        getline(cin, trash);

        vector<queue<string>> grid(n);
        readGrid(grid);
        writeGrid(grid);
    }
}

void spawnTableGrid()
{
    int pipe_fd[2];
    pipe(pipe_fd);
    int f = fork();
    if (f > 0)
    {
        dup2(pipe_fd[0],0);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        tableGridExecute();
        exit(0);
    }
    dup2(pipe_fd[1],1);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
}

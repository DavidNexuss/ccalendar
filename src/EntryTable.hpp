#pragma once
#include "Entry.hpp"
#include <vector>
struct EntryTable
{
    std::vector<Entry> entries;
    std::vector<int> important_days;
    
    EntryTable();
    void readEntryTable();
    void printCalendar(const std::string& cal) const;
    void printEntryTable(int i = 0,int offset = 0) const;
};

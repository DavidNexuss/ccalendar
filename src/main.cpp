#include <iostream>
#include <unistd.h>
#include "EntryTable.hpp"
#include "Calendar.hpp"
#include "TablePrint.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc > 1 && string(argv[1]) == "-c") calendar.useConky = true;
    calendar.readColors();
    time_t t = time(NULL);
    calendar.currentDay = *localtime(&t);
    EntryTable table;
    table.readEntryTable();
    table.printCalendar(getCalendar());
}

#include <iostream>
#include <ctime>

#include "w_msg.h"

using std::cout;
using std::endl;
using std::cerr;


W_Msg::W_Msg()
{

}

void W_Msg::Error(const string & msg)
{
    cerr << CurDate() << msg << endl; 
}

void W_Msg::Error(const char * msg)
{
    cerr << CurDate() << msg << endl; 
}


void W_Msg::Debug(const string & msg)
{
     cout << CurDate() << msg << endl;
}

void W_Msg::Debug(const char * msg)
{
    cout << CurDate() << msg << endl; 
}

void W_Msg::Fatal(const string & msg)
{
    cerr << CurDate() << msg << endl; 
}

void W_Msg::Fatal(const char * msg)
{
    cerr << CurDate() << msg << endl; 
}


string W_Msg::CurDate()
{
    time_t timep;
    time (&timep);
    //string date(asctime(gmtime(&timep)));
    string date(ctime(&timep));
    date.replace(date.end() - 1, date.end(), "\t");

    return date;
}

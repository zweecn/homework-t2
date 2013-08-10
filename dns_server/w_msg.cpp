#include <iostream>
#include <ctime>
#include <memory>
#include <cstring>

#include "w_msg.h"


W_Msg::W_Msg()
{

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

string W_Msg::CurLine(const char* funcName, const char* file, int line)
{
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "\tFUN:%s FILE:%s LINE:%d\n", funcName, file, line);
   
    return string(buf);
}

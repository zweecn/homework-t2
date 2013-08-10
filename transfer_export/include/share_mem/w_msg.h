#ifndef __W_MSG_H_
#define __W_MSG_H_

#include <string>

using std::string;

class W_Msg
{
public:
    explicit W_Msg();
    static void Error(const string & msg);
    static void Error(const char* msg);

    static void Debug(const string & msg);
    static void Debug(const char* msg);

    static void Fatal(const string & msg);
    static void Fatal(const char* msg);
    
    static string CurDate();

private:
};


#endif

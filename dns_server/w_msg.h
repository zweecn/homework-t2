#ifndef __W_MSG_H_
#define __W_MSG_H_

#include <string>
#include <cstdio>
#include <cstdlib>

using std::string;

#define DEBUG_MSG (std::cout << W_Msg::CurDate() << "DEBUG:\t")
#define ERROR_MSG (std::cerr << W_Msg::CurDate() << "ERROR:\t")
#define FATAL_MSG (std::cerr << W_Msg::CurDate() << "FATAL:\t")

#define MSG_POS (W_Msg::CurLine(__FUNCTION__, __FILE__, __LINE__))


class W_Msg
{
public:
    explicit W_Msg();
    static string CurDate();
    static string CurLine(const char* funcName, const char* file, int line);
private:
};

#endif

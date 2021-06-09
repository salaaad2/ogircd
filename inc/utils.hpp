#ifndef __UTILS_H_
#define __UTILS_H_

#include <string>
#include <sys/types.h>
#include <ctime>

using std::string;

string u_utoa(unsigned int n);
string u_format_cmd(string cmd);
string u_current_time(void);
string u_uptime(time_t &launch_time);
bool u_strmatch(string s, string p);

#endif // __UTILS_H_

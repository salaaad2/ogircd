#ifndef __UTILS_H_
#define __UTILS_H_

#include <string.h>
#include <string>
#include <sys/types.h>
#include <ctime>

using std::string;

void ft_bzero(void *s, size_t n);
int ft_isalpha(const char c);
int ft_isnum(const char c);
string ft_utoa(unsigned int n);
string ft_format_cmd(string cmd);
string ft_current_time(void);
string ft_uptime(time_t &launch_time);
bool strmatch(string s, string p);

#endif // __UTILS_H_

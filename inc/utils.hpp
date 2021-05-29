#ifndef __UTILS_H_
#define __UTILS_H_

#include <string.h>
#include <string>
#include <sys/types.h>
#include <ctime>

int         ft_atoi(const char *str);
void        ft_bzero(void *s, size_t n);
int         ft_isalpha(const char c);
int         ft_isnum(const char c);
std::string ft_utoa(unsigned int n);
std::string ft_format_cmd(std::string cmd);
std::string ft_current_time(void);
std::string ft_uptime(time_t &launch_time);
bool strmatch(std::string s, std::string p);


#endif // __UTILS_H_

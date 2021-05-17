#ifndef __UTILS_H_
#define __UTILS_H_

#include <string.h>
#include <string>
#include <sys/types.h>

int	        ft_atoi(const char *str);
void	    ft_bzero(void *s, size_t n);
int         ft_isalpha(const char c);
int         ft_isnum(const char c);
std::string ft_utoa(unsigned int n);
std::string ft_format_cmd(std::string cmd);


#endif // __UTILS_H_

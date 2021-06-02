#include "../inc/ftirc.hpp"

int	ft_atoi(const char *str)
{
    int i;
    int s;
    int nb;

    i = 0;
    s = 1;
    nb = 0;
    while (str[i] == ' ' || str[i] == '\t'
           || str[i] == '\n' || str[i] == '\f'
           || str[i] == '\r' || str[i] == '\v')
        i++;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-')
            s = -1;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        nb = nb * 10 + (str[i] - '0');
        i++;
    }
    return (s * nb);
}

void	ft_bzero(void *s, size_t n)
{
    unsigned char	*str;
    size_t			i;

    i = 0;
    str = (unsigned char *)s;
    while (i < n)
    {
        *str = 0;
        str++;
        i++;
    }
}

int ft_isalpha(const char c) {
    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
        return (1);
    else
        return (0);
}

int ft_isnum(const char c)
{
    if (c >= 48 && c <= 57)
        return (1);
    else
        return (0);
}

size_t  ft_strlen(const char *src) 
{
    size_t i = 0;

    while (src[i])
        i++;
    return i;
}

size_t  ft_strlcpy(char *dst, const char *src, size_t size)
{
    size_t  i;

    if (!dst || !src)
        return (0);
    i = 0;
    if (size == 0)
        return (ft_strlen(src));
    while (i < size - 1 && src[i])
    {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return (ft_strlen(src));
}

std::string ft_utoa(unsigned int n)
{
    std::string str;
    size_t i;

    if (n == 0)
        return (std::string("0"));
    i = n;
    while (i)
    {
        str.insert(str.begin(), (i % 10) + 48);
        i /= 10;
    }
    return (str);
}

std::string ft_format_cmd(std::string cmd)
{
    while (cmd.size() < 3)
        cmd.insert(cmd.begin(), '0');
    return (cmd);
}

std::string ft_current_time(void)
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    return(asctime(timeinfo));

}

std::string ft_uptime(time_t &launch_time)
{
    time_t rawtime;
    time_t uptime;
    struct tm *s_uptime;
    time (&rawtime);
    uptime = rawtime - launch_time;
    s_uptime = localtime(&uptime);
    return(ft_utoa(s_uptime->tm_mday - 1) + " days " + ft_utoa(s_uptime->tm_hour - 1) + ":" + ft_utoa(s_uptime->tm_min) + ":" + ft_utoa(s_uptime->tm_sec));
}

int finding(std::vector<std::vector<int> > &dp, std::string& s, std::string& p, int n, int m)
{
    // return 1 if n and m are negative
    if (n < 0 && m < 0)
        return 1;
   
    // return 0 if m is negative
    if (m < 0)
        return 0;
   
    // return n if n is negative
    if (n < 0) 
    {
        // while m is positve
        while (m >= 0) 
        {
            if (p[m] != '*')
                return 0;
            m--;
        }
        return 1;
    }
    
    // if dp state is not visited
    if (dp[n][m] == -1) 
    {
        if (p[m] == '*')
        {
            return dp[n][m] = finding(dp, s, p, n - 1, m)
                              || finding(dp, s, p, n, m - 1);
        }
        else
        {
            if (p[m] != s[n] && p[m] != '?')
                return dp[n][m] = 0;
            else
                return dp[n][m]
                       = finding(dp, s, p, n - 1, m - 1);
        }
    }
   
    // return dp[n][m] if dp state is previsited
    return dp[n][m];
}
 
 
bool isMatch(std::vector<std::vector<int> > &dp, std::string s, std::string p)
{
    dp.clear();
     
    // resize the dp array
    dp.resize(s.size() + 1, std::vector<int>(p.size() + 1, -1));
    return dp[s.size()][p.size()]
           = finding(dp, s, p, s.size() - 1, p.size() - 1);
}

bool strmatch(std::string s, std::string p)
{
    std::vector<std::vector<int> > dp;
    return isMatch(dp, s, p);
}

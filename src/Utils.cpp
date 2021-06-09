#include "../inc/ftirc.hpp"

string u_utoa(unsigned int n)
{
	string str;
	size_t i;

	if (n == 0)
		return (string("0"));
	i = n;
	while (i) {
		str.insert(str.begin(), (i % 10) + 48);
		i /= 10;
	}
	return (str);
}

string u_format_cmd(string cmd)
{
	while (cmd.size() < 3)
		cmd.insert(cmd.begin(), '0');
	return (cmd);
}

string u_current_time(void)
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return (asctime(timeinfo));
}

string u_uptime(time_t &launch_time)
{
	time_t rawtime;
	time_t uptime;
	struct tm *s_uptime;
	time(&rawtime);
	uptime = rawtime - launch_time;
	s_uptime = localtime(&uptime);
	return (u_utoa(s_uptime->tm_mday - 1) + " days " +
		u_utoa(s_uptime->tm_hour - 1) + ":" +
		u_utoa(s_uptime->tm_min) + ":" + u_utoa(s_uptime->tm_sec));
}

int finding(std::vector<std::vector<int> > &dp, string &s, string &p,
	    int n, int m)
{
	// return 1 if n and m are negative
	if (n < 0 && m < 0)
		return 1;

	// return 0 if m is negative
	if (m < 0)
		return 0;

	// return n if n is negative
	if (n < 0) {
		// while m is positve
		while (m >= 0) {
			if (p[m] != '*')
				return 0;
			m--;
		}
		return 1;
	}

	// if dp state is not visited
	if (dp[n][m] == -1) {
		if (p[m] == '*') {
			return dp[n][m] = finding(dp, s, p, n - 1, m) ||
					  finding(dp, s, p, n, m - 1);
		} else {
			if (p[m] != s[n] && p[m] != '?')
				return dp[n][m] = 0;
			else
				return dp[n][m] =
					       finding(dp, s, p, n - 1, m - 1);
		}
	}

	// return dp[n][m] if dp state is previsited
	return dp[n][m];
}

bool isMatch(std::vector<std::vector<int> > &dp, string s, string p)
{
	dp.clear();

	// resize the dp array
	dp.resize(s.size() + 1, std::vector<int>(p.size() + 1, -1));
	return dp[s.size()][p.size()] =
		       finding(dp, s, p, s.size() - 1, p.size() - 1);
}

bool u_strmatch(string s, string p)
{
	std::vector<std::vector<int> > dp;
	return isMatch(dp, s, p);
}

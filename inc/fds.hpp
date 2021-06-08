#ifndef __FDS_H_
#define __FDS_H_

#include <sys/types.h>

struct Fds {
	fd_set master;
	fd_set read;
	int fdmax;
};

#endif // __FDS_H_

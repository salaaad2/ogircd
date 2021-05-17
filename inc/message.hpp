#ifndef __MESSAGE_H_
#define __MESSAGE_H_

struct Message
{
    char    command[64];
    char    params[32][32];
};


char    *msg_error(int code);
#endif // __MESSAGE_H_

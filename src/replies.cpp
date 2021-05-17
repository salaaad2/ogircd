#include "../inc/replies.hpp"

const char *msg_rpl(int code)
{
    switch(code){
        case RPL_WELCOME :
            return("WELCOME");
        default :
            return ("");
    }
}

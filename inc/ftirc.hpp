#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "replies.hpp"
#include "fds.hpp"
#include "Message.hpp"
#include "Client.hpp"
#include "Server.hpp"

void rec_data(Server &serv, int i, Fds *fdmax);

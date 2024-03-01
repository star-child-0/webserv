#pragma once

#include <iostream>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

enum HTTP_METHOD {
	GET,
	POST,
	DELETE
};

#include "Server.h"
class Server;

#include "Log.h"
#include "Error.h"

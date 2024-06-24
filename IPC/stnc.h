#pragma once

#include <netinet/tcp.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

#include "sidefunctions.h"

void startClient(char *ip, char *port);
void startServer(char *port);


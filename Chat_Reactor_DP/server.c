#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <errno.h>

#include "reactor.h"

#define PORT "9034"   // Port we're listening on

Reactor *reactor;

void handleCtrlZ(int signal) {
    stopReactor(reactor);
    exit(0);
}

void handleCtrlC(int signal) {
    stopReactor(reactor);
    exit(0);
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

void handle_new_connection(Reactor *reactor, int fd);
void handle_client(Reactor *reactor, int newfd);

void handle_new_connection(Reactor *reactor, int fd) {
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof (remoteaddr);
    newfd = accept(fd, (struct sockaddr *) &remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        printf("server: new connection from %s on socket %d\n",
               inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *) &remoteaddr), remoteIP, INET6_ADDRSTRLEN),
               newfd);

        addFd(reactor, newfd, handle_client);
    }
}

void handle_client(Reactor *reactor, int newfd) {
    char buf[256];
    int nbytes;

    nbytes = recv(newfd, buf, sizeof buf, 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            printf("server: socket %d hung up\n", newfd);
        } else {
            perror("recv");
        }
        close(newfd);

        removeFd(reactor, newfd);

    } else {
        for (int j = 0; j < reactor->size; j++) {
            int dest_fd = reactor->fds[j].fd;

            if (dest_fd != newfd && dest_fd != reactor->fds[0].fd) {
                if (send(dest_fd, buf, nbytes, 0) == -1) {
                    perror("send");
                }
            }
        }
    }
}
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}
int main(void) {
    int listener;

    listener = get_listener_socket();
    if (listener == -1) {
        perror("Error in get_listener_socket()");
        exit(EXIT_FAILURE);
    }

    reactor = createReactor();
    addFd(reactor, listener, handle_new_connection);

    signal(SIGTSTP, handleCtrlZ);
    signal(SIGINT, handleCtrlC);

    startReactor(reactor);

    while(reactor->running) {
        sleep(1);
    }

    return 0;
}
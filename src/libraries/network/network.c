#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int network_init(NetworkManager *net, char * ip, char * port)
{
    struct addrinfo hints, *servinfo, *p;
    int status;
    int yes = 1;
    char ipstr[INET6_ADDRSTRLEN]; 

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((net->fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("network: socket");
            continue;
        }
        // fcntl(net->fd, F_SETFL, O_NONBLOCK);
        if (setsockopt(net->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        if (net->protocol == SERVER){
            if (bind(net->fd, p->ai_addr, p->ai_addrlen) == -1){
                perror("network: Bind");
                return -1;
            }
            break;
        }
        net->addr_len = p->ai_addrlen;
        memcpy(&net->addr, p->ai_addr, net->addr_len);

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "init network failed\n");
        return -1;
    }
    if (net->func == CLIENT)
    {
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof ipstr);
        printf("client: connecting to %s port %s\n", ipstr, port);
    }

    freeaddrinfo(servinfo); // all done with this structure

    return 1;
}

int network_connect(NetworkManager *net)
{
    if (net->protocol == TCP){
        if (connect(net->fd, &net->addr, net->addr_len) == -1)
        {
            perror("network: connect");
        }
        return 1;
    }
}

int network_read(NetworkManager *net, char * buf, int len)
{
    int bytes;
    memset(buf, '\0', len);
    if ((bytes = recv(net->fd, buf, len-1, 0)) != -1)
    {
        printf("client: received '%s'\n",buf);
        printf("client: received '%d bytes'\n",bytes);
    }
    
    return bytes;
}

int network_deinit(NetworkManager *net)
{
    close(net->fd);

    return 1;
}

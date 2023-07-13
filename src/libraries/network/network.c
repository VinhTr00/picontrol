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
    hints.ai_socktype = (net->protocol == TCP) ? SOCK_STREAM : SOCK_DGRAM;

    if ((status = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((net->fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("network init (socket)");
            continue;
        }
        /* --- INIT NON-BLOCKING --- */
        int flags = fcntl(net->fd, F_GETFL, 0);
        if (flags == -1) {
            perror("network init (fcntl)");
            return -1;
        }
        if (fcntl(net->fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            perror("network init (fcntl)");
            return -1;
        }
        /* -------------------------- */
        
        if (setsockopt(net->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("network init (setsockopt)");
            return -1;
        }
        if (net->model == SERVER){
            if (bind(net->fd, p->ai_addr, p->ai_addrlen) == -1){
                perror("network init (Bind)");
                return -1;
            }
            break;
        }
        net->addr_len = p->ai_addrlen;
        memcpy(&net->addr, p->ai_addr, net->addr_len);

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "network init (failed)\n");
        return -1;
    }

    if (net->model == CLIENT)
    {
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr, sizeof ipstr);
        printf("client: connecting to %s port %s\n", ipstr, port);
    }

    freeaddrinfo(servinfo); // all done with this structure

    net->pfds = calloc(1, sizeof(struct pollfd));
    return 1;
}

int network_connect(NetworkManager *net)
{
    if (net->protocol == TCP)
    {
        if (connect(net->fd, &net->addr, net->addr_len) == -1  && errno != EINPROGRESS)
        {
            perror("network connect (failed)");
            return -1;
        }

        net->pfds[0].fd = net->fd;
        net->pfds[0].events = POLLOUT;
        int ready = poll(net->pfds, 1, 1000);

        if (ready == -1) 
        {
            perror("network connect (poll)");
            return -1;
        }
        else if (ready == 0) 
        {
            fprintf(stderr, "network connect (timeout)\n");
            return -1;
        }

        int err;
        socklen_t errlen = sizeof(err);
        if (getsockopt(net->fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) 
        {
            perror("network connect (getsockopt)");
            return -1;
        }
        if (err != 0) 
        {
            errno = err;
            perror("network connect (failed)");
            return -1;
        }

    }
    else 
    {
        int numbytes;
        char buf[10] = "Connected";
        if ((numbytes = sendto(net->fd, buf, strlen(buf), 0, &net->addr, net->addr_len)) == -1) {
            perror("network UDP sendto failed");
            return -1;
        }
    }
    printf("network connect: success\n");
    return 1;
}

int network_read(NetworkManager *net, char * buf, int len, int timeout)
{
    int bytes = 0;
    memset(buf, '\0', len);

    net->pfds[0].fd = net->fd;
    net->pfds[0].events = POLLIN;
    int poll_count = poll(net->pfds, 1, timeout * 1000); 

    if (poll_count == -1) {
        perror("network read (poll)");
        return -1;
    }
    else if (poll_count == 0) {
        fprintf(stderr, "read timeout\n");
        return -1;
    }
    
    if (net->pfds[0].revents & POLLIN){
        switch (net->protocol)
        {
            case TCP:
                bytes = recv(net->fd, buf, len, 0);
                break;
            case UDP:
                bytes = recvfrom(net->fd, buf, len, 0, &net->addr, &net->addr_len);
                break;
        }   
    } 

    else if (bytes < 0 )
    {
        perror("network read (recv)");
        return -1;

    }
    // if (bytes > 0) {
    //     printf("network read: %d bytes\n", bytes);
    //     printf("network read: '%s'\n", buf);
    // }
    
    return bytes;
}
int network_write(NetworkManager *net, char * buf, int len, int timeout)
{
    int bytes = 0;
    net->pfds[0].fd = net->fd;
    net->pfds[0].events = POLLOUT;
    int poll_count = poll(net->pfds, 1, timeout * 1000);
    if (poll_count == -1) {
        perror("network write (poll)");
        return -1;
    }
    else if (poll_count == 0) {
        fprintf(stderr, "write timeout\n");
        return -1;
    }
    if (net->pfds[0].revents & POLLOUT){
        switch (net->protocol)
        {
            case TCP:
                bytes = send(net->fd, buf, len, 0);
                break;
            case UDP:
                bytes = sendto(net->fd, buf, len, 0, &net->addr, net->addr_len);
                break;
        }   
    }
    else if (bytes < 0) 
    {
        perror("network write (send)");
        return -1;
    }
    if (bytes > 0)
    {
        printf("network write: %d bytes\n", bytes);
        // printf("network write: '%s'\n", buf);
    }
    return bytes;
}

int network_deinit(NetworkManager *net)
{
    close(net->fd);
    return 1;
}

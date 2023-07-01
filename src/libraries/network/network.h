#ifndef __NETWORK_H
#define __NETWORK_H

#include <sys/types.h>
#include <netinet/in.h>

typedef enum {
	TCP = 0,
	UDP
} Protocol;

typedef enum 
{
    CLIENT,
    SERVER,
} Functional;

typedef struct 
{
    int fd;
    Protocol protocol;
    Functional func;
    struct sockaddr addr;
    socklen_t addr_len;
} NetworkManager;

#ifdef __cplusplus
extern "C" {
#endif

extern int network_init(NetworkManager *net, char * ip, char * port);
extern int network_connect(NetworkManager *net);
extern int network_write(NetworkManager *net);
extern int network_read(NetworkManager *net, char * buf, int len);
extern int network_deinit(NetworkManager *net);

#ifdef __cplusplus
}
#endif

#endif /* __NETWORK_H */
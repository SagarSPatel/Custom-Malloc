#ifndef _LIBNETFILES_H
#define _LIBNETFILES_H

#define netopen(a,b) netopen(a,b)

#define netclose(a) netclose(a)

#define netwrite(a,b,c) netwrite(a,b,c)

#define netread(a,b,c) netread(a,b,c)

#define netseverinit(a) netserverinit(a)

#define netprint() netprint()

netprint();

int netopen(char *path, int flags);

int netclose(int fd);

ssize_t netread(int fildes, void *buf ,size_t nbyte);

ssize_t netwrite(int fildes, void *buf,size_t nbyte);
 
netserverinit(char *a);

#endif

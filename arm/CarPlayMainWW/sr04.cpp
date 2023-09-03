#include "sr04.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <poll.h>

int sr04_fd = 0;

int sr04_open(const char* devname)
{
    sr04_fd = open(devname, O_RDWR);
    if (sr04_fd == -1)
    {
        printf("can not open file %s\n", devname);
        return -1;
    }
    return 0;
}
int sr04_ioctl(unsigned int cmd, unsigned long arg)
{
    ioctl(sr04_fd, cmd, arg);
    return 0;
}
int sr04_poll(struct pollfd* fds, int nfds, int timeout)
{
    return poll(fds, nfds, timeout);
}
int sr04_read(void* buf, int nBytes)
{
    return read(sr04_fd, buf, nBytes);
}
void sr04_close(void)
{
    if(sr04_fd) close(sr04_fd);
}

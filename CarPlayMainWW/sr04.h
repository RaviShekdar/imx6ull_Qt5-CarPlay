#ifndef SR04_H
#define SR04_H

#ifdef __cplusplus
extern "C"
{
#endif

    extern int sr04_open(const char* devname);
    extern int sr04_ioctl(unsigned int cmd, unsigned long arg);
    extern int sr04_read(void* buf, int nBytes);
    extern void sr04_close(void);
    extern int sr04_poll(struct pollfd* fds, int nfds, int timeout);
    extern int sr04_fd;

#ifdef __cplusplus
}
#endif

#endif // SR04_H

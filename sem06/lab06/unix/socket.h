#ifndef SOCKET_H_
#define SOCKET_H_

#define SOCKET_NAME "socket.soc"
#define BUF_SIZE 256

/* defined in linux/kernel.h */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#endif  // SOCKET_H_

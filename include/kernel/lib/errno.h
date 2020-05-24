#ifndef __ERRNO_H__
#define __ERRNO_H__

typedef enum {
    GENERIC_ERR = -1,
    GENERIC_SUCCESS = 0
} KernelResult;

/* TODO: add an enum for specific errors */

extern volatile int errno;

#endif /* __ERRNO_H__ */

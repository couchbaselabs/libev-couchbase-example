#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include <libcouchbase/couchbase.h>
#include "ev.h"
#include <fcntl.h>
#include "ringbuffer/ringbuffer.h"

struct client_s {
    struct ev_io io;
    ringbuffer_t in;
    ringbuffer_t out;
    struct ev_loop *loop;
    int fd;
    struct sockaddr_in addr;
    socklen_t naddr;
    lcb_t handle;
};

struct server_s {
    struct ev_io io;
    lcb_t handle;
};

#define fail(msg)   \
    perror(msg);    \
    abort();

#define BUFFER_SIZE 256

#endif

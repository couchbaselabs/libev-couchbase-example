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

lcb_t storage_init(struct ev_loop *loop, const char *host, const char *bucket, const char *password);
void storage_put(struct client_s *client, const char *key, const void *val, size_t nval);

#define fail(msg)   \
    perror(msg);    \
    abort();

#define BUFFER_SIZE 256

#endif

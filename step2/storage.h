#ifndef STORAGE_H
#define STORAGE_H

#include "common.h"

lcb_t storage_init(struct ev_loop *loop, const char *host, const char *bucket, const char *password);
void storage_put(struct client_s *client, const char *key, const void *val, size_t nval);

#endif

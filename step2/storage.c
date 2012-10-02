/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "storage.h"

#undef fail
#define fail(msg)                   \
    fprintf(stderr, "%s\n", msg);   \
    abort();

static void
error_callback(lcb_t instance, lcb_error_t err, const char *errinfo)
{
    fprintf(stderr, "%s (0x%x): %s\n", lcb_strerror(instance, err), err, errinfo);
    fail("libcouchbase error");
}

static void
storage_callback(lcb_t instance, const void *cookie,
                 lcb_storage_t operation, lcb_error_t error,
                 const lcb_store_resp_t *resp)
{
    struct client_s *client = (struct client_s *)cookie;
    struct ev_io* watcher = (struct ev_io*)client;
    char cas_str[128];
    ssize_t nb;

    nb = snprintf(cas_str, 128, "%"PRIu64"\n", resp->v.v0.cas);
    if (nb < 0) {
        fail("output CAS value");
    }
    ringbuffer_ensure_capacity(&client->out, nb);
    if (ringbuffer_write(&client->out, cas_str, nb) != nb) {
        fail("write CAS into the buffer");
    }
    ev_io_stop(client->loop, watcher);
    ev_io_set(watcher, watcher->fd, EV_WRITE);
    ev_io_start(client->loop, watcher);

    (void)operation;
    (void)resp;
}

lcb_t
storage_init(struct ev_loop *loop, const char *host, const char *bucket, const char *password)
{
    struct lcb_create_st opts;
    struct lcb_create_io_ops_st io_opts;
    lcb_t handle;
    lcb_error_t err;

    io_opts.version = 1;
    io_opts.v.v1.sofile = NULL;
    io_opts.v.v1.symbol = "lcb_create_libev_io_opts";
    io_opts.v.v1.cookie = loop;

    opts.version = 0;
    opts.v.v0.host = host;
    opts.v.v0.bucket = bucket;
    opts.v.v0.user = bucket;
    opts.v.v0.passwd = password;

    err = lcb_create_io_ops(&opts.v.v0.io, &io_opts);
    if (err != LCB_SUCCESS) {
        error_callback(NULL, err, "failed to create IO object");
        return NULL;
    }
    err = lcb_create(&handle, &opts);
    if (err != LCB_SUCCESS) {
        error_callback(NULL, err, "failed to create connection object");
        return NULL;
    }

    (void)lcb_set_error_callback(handle, error_callback);
    (void)lcb_set_store_callback(handle, storage_callback);

    err = lcb_connect(handle);
    if (err != LCB_SUCCESS) {
        error_callback(handle, err, "failed to connect to the server");
        return NULL;
    }

    return handle;
}

void
storage_put(struct client_s *client, const char *key, const void *val, size_t nval)
{
    lcb_error_t err;
    lcb_store_cmd_t cmd;
    const lcb_store_cmd_t *cmds[] = { &cmd };

    memset(&cmd, 0, sizeof(cmd));
    cmd.version = 0;
    cmd.v.v0.key = key;
    cmd.v.v0.nkey = strlen(key);
    cmd.v.v0.bytes = val;
    cmd.v.v0.nbytes = nval;
    cmd.v.v0.operation = LCB_SET;
    err = lcb_store(client->handle, client, 1, cmds);
    if (err != LCB_SUCCESS) {
        error_callback(client->handle, err, "failed to schedule store operation");
    }
}

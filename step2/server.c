/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#include "common.h"

#define PORT_NO 4567
#define THE_KEY "example"

static ssize_t
io_recvv(lcb_socket_t sock, struct lcb_iovec_st iov[])
{
    struct msghdr msg;
    struct iovec msg_iov[2];

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = msg_iov;
    msg.msg_iovlen = iov[1].iov_len ? (size_t)2 : (size_t)1;
    msg.msg_iov[0].iov_base = iov[0].iov_base;
    msg.msg_iov[0].iov_len = iov[0].iov_len;
    msg.msg_iov[1].iov_base = iov[1].iov_base;
    msg.msg_iov[1].iov_len = iov[1].iov_len;
    return recvmsg(sock, &msg, 0);
}

static ssize_t
io_sendv(lcb_socket_t sock, struct lcb_iovec_st iov[])
{
    struct msghdr msg;
    struct iovec msg_iov[2];

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = msg_iov;
    msg.msg_iovlen = iov[1].iov_len ? (size_t)2 : (size_t)1;
    msg.msg_iov[0].iov_base = iov[0].iov_base;
    msg.msg_iov[0].iov_len = iov[0].iov_len;
    msg.msg_iov[1].iov_base = iov[1].iov_base;
    msg.msg_iov[1].iov_len = iov[1].iov_len;
    return sendmsg(sock, &msg, 0);
}

void
client_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct lcb_iovec_st iov[2];
    ssize_t nbytes;
    struct client_s *client = (struct client_s *)watcher;

    if (EV_READ & revents) {
        ringbuffer_ensure_capacity(&client->in, BUFFER_SIZE);
        ringbuffer_get_iov(&client->in, RINGBUFFER_WRITE, iov);
        nbytes = io_recvv(watcher->fd, iov);
        if (nbytes < 0) {
            fail("read error");
        } else if (nbytes == 0) {
            ev_io_stop(loop, watcher);
            ringbuffer_destruct(&client->in);
            free(client);
            printf("Peer disconnected\n");
            return;
        } else {
            char *val;
            ringbuffer_produced(&client->in, nbytes);
            printf("Received %zu bytes\n", nbytes);
            val = malloc(nbytes);
            if (!val) {
                fail("allocate buffer for value");
            }
            if (ringbuffer_read(&client->in, val, nbytes) != nbytes) {
                free(val);
                fail("read value into the buffer");
            }
            storage_put(client, THE_KEY, val, nbytes);
            free(val);
        }
    } else if (EV_WRITE & revents) {
        ringbuffer_get_iov(&client->out, RINGBUFFER_READ, iov);
        nbytes = io_sendv(watcher->fd, iov);
        if (nbytes < 0) {
            fail("write error");
        } else if (nbytes == 0) {
            ev_io_stop(loop, watcher);
            ringbuffer_destruct(&client->out);
            free(client);
            printf("Peer disconnected\n");
            return;
        } else {
            ringbuffer_consumed(&client->out, nbytes);
            printf("Sent %zu bytes\n", nbytes);
            ev_io_stop(loop, watcher);
            ev_io_set(watcher, watcher->fd, EV_READ);
            ev_io_start(loop, watcher);
        }
    } else  {
        fail("got invalid event");
    }
}

void
accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct client_s *client;
    struct server_s *server = (struct server_s *)watcher;

    if (EV_ERROR & revents) {
        fail("got invalid event");
    }
    client = malloc(sizeof(struct client_s));
    if (!client) {
        fail("client watcher alloc");
    }
    if (!ringbuffer_initialize(&client->in, BUFFER_SIZE)) {
        fail("client input buffer alloc");
    }
    if (!ringbuffer_initialize(&client->out, BUFFER_SIZE)) {
        fail("client output buffer alloc");
    }
    client->naddr = sizeof(client->addr);
    client->fd = accept(watcher->fd, (struct sockaddr *)&client->addr,
                        &client->naddr);
    client->loop = loop;
    client->handle = server->handle;
    if (client->fd < 0) {
        fail("accept error");
    }

    printf("Successfully connected with client\n");

    ev_io_init((ev_io *)client, client_cb, client->fd, EV_READ);
    ev_io_start(loop, (ev_io *)client);
}

int
main()
{
    struct ev_loop *loop = ev_default_loop(0);
    int sd;
    struct sockaddr_in addr;
    struct server_s server;

    server.handle = storage_init(loop, "localhost:8091", "default", NULL);

    if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        fail("socket error");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NO);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        fail("bind error");
    }
    if (listen(sd, 2) < 0) {
        fail("listen error");
    }

    ev_io_init(&server.io, accept_cb, sd, EV_READ);
    ev_io_start(loop, &server.io);

    printf("Listen on %d\n", PORT_NO);
    ev_run(loop, 0);

    return 0;
}

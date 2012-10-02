# Integration Demo

This demo shows how to integrate libcouchbase with the application which
already use its own event loop, moreover, this event loop (in the
example libev) built-in statically into host application.

In the directory `step1` stored the application before integration:
simple echo server which unlike popular echo server examples accessible
in the internets, doesn't make an assumption that socket is writable on
EV_READ event. Therefore it just copy received data into the ringbuffer
and setup the write watcher.

In the directory `step2` you can find almost the same application, but
it is forwarding incoming message to couchbase server and reply its CAS
value back to the client. The interesting thing here, that libcouchbase
could be used without any event library dependency, because the host
server application already has builtin event loop.

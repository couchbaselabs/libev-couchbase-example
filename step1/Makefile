CFLAGS=-I. -Werror -Wall -Wextra
LDFLAGS=-lev -lcouchbase

server: server.c ringbuffer/ringbuffer.c ringbuffer/ringbuffer.h
	${CC} ${CFLAGS} ${LDFLAGS} -o server server.c ringbuffer/ringbuffer.c

reformat:
	astyle --mode=c \
               --quiet \
               --style=1tbs \
               --indent=spaces=4 \
               --indent-namespaces \
               --indent-col1-comments \
               --max-instatement-indent=78 \
               --pad-oper \
               --pad-header \
               --add-brackets \
               --unpad-paren \
               --align-pointer=name \
               --align-reference=name \
               *.[ch]

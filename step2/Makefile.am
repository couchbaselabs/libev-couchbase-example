
bin_PROGRAMS = server
server_CPPFLAGS = -I${top_srcdir}/libev -I${top_srcdir}/lcb-plugin
server_LDFLAGS = -ldl -export-dynamic
server_SOURCES = \
				 lcb-plugin/plugin-libev.c \
				 libev/ev.c \
				 libev/ev.h \
				 ringbuffer/ringbuffer.c \
				 ringbuffer/ringbuffer.h \
				 server.c \
				 storage.c

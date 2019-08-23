AM_CFLAGS = -I$(top_srcdir)/server/include
AM_CFLAGS += -I$(top_srcdir)/shared/include

bin_PROGRAMS = server
server_SOURCES = main.c

server_LDADD = ../lib/libserver.la
server_LDADD += ../lib/libgame.la
server_LDADD += ../lib/libbomb.la
server_LDADD += $(top_builddir)/shared/src/libutils.la